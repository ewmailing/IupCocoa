
------------------------------------------------------------------------------
-- Callback handler  
------------------------------------------------------------------------------

iup.callbacks = {} -- storage for the C callbacks

function iup.CallMethod(name, ...)
  local ih = ... -- the first argument is always the iupHandle
  local lua_func = ih[name] -- use "gettable" to retrieve the Lua callback
  if (not lua_func) then
    return
  end
  
  if type(lua_func) == "function" then
    return lua_func(...)
  elseif type(lua_func) == "string" then  
    local temp = self
    self = ih
    local result = iup.dostring(lua_func)
    self = temp
    return result
  else
    return iup.ERROR
  end
end

function iup.RegisterCallback(name, c_func, type)
  -- Store a C callback for future use
  if not iup.callbacks[name] then iup.callbacks[name] = {} end
  local cb = iup.callbacks[name]
  if type then
    cb[type] = c_func
  else
    cb[1] = c_func
  end
end

function iup.CallGlobalMethod(name, ...)
  local INDEX = string.upper(name)
  local cb = iup.callbacks[INDEX]
  if (not cb) then 
    return
  end

  local lua_func = cb[2]
  if type(lua_func) == "function" then
    return lua_func(...)
  elseif type(lua_func) == "string" then  
    return iup.dostring(lua_func)
  else
    return iup.ERROR
  end
end

function iup.SetGlobalCallback(name, lua_func)
  local INDEX = string.upper(name)
  local cb = iup.callbacks[INDEX]
  if (cb) then -- if a callback name
    if (lua_func) then
      local c_func = cb[1]
      iup.SetFunction(INDEX, c_func) -- set the pre-defined C callback
    else
      iup.SetFunction(INDEX, nil)
    end
    cb[2] = lua_func -- store also the Lua callback
  end
end


------------------------------------------------------------------------------
-- Meta Methods 
------------------------------------------------------------------------------

------------------------------------------------------------------------------
-- this is a Lua table for control construction, see iup.WIDGET and iup.BOX
-- not used by applications

-- implements class inheritance for iupWidget
local widget_gettable = function(widget, index)
  local p = widget
  local v
  while p do
    v = rawget(p, index)
    if v then 
      return v 
    end

    p = rawget(p, "parent")
  end
  return nil
end

iup.NewClass("iupWidget")
iup.SetMethod("iupWidget", "__index", widget_gettable)


------------------------------------------------------------------------------
-- this is an Ihandle* with enhancements

local ihandle_gettable = function(ih, index)
  local INDEX = string.upper(index)
  if (iup.callbacks[INDEX]) then 
    local widget = iup.GetWidget(ih)
    if (not widget or type(widget)~="table") then error("invalid IUP handle") end
    return widget[index]
  else
    local value = iup.GetAttribute(ih, INDEX)
    if (not value) then
      local widget = iup.GetWidget(ih)
      if (not widget or type(widget)~="table") then error("invalid IUP handle") end
      return widget[index]
    elseif type(value)== "number" or type(value) == "string" then
      local ih = iup.GetHandle(value)
      if ih then 
        return ih
      else 
        return value 
      end
    else
      return value 
    end
  end
end

local ihandle_settable = function(ih, index, value)
  local ti = type(index)
  local tv = type(value)
  local widget = iup.GetWidget(ih)
  if (not widget or type(widget)~="table") then error("invalid IUP handle") end
  if ti == "number" or ti == "string" then -- check if a valid C name
    local INDEX = string.upper(index)
    local cb = iup.callbacks[INDEX]
    if (cb) then -- if a callback name
      local c_func = cb[1]
      if (not c_func) then
        c_func = cb[iup.GetClassName(ih)]
      end
      iup.SetCallback(ih, INDEX, c_func, value) -- set the pre-defined C callback
      widget[index] = value -- store also in Lua
    elseif iup.GetClass(value) == "iupHandle" then -- if an iupHandle
      local name = iup.SetHandleName(value)
      iup.SetAttribute(ih, INDEX, name)
      widget[index] = nil -- if there was something in Lua remove it
    elseif tv == "string" or tv == "number" or tv == "nil" then -- if a common value
      iup.SetAttribute(ih, INDEX, value)
      widget[index] = nil -- if there was something in Lua remove it
    else
      widget[index] = value -- store only in Lua
    end
  else
    widget[index] = value -- store only in Lua
  end
end

iup.NewClass("iupHandle")
iup.SetMethod("iupHandle", "__index", ihandle_gettable)
iup.SetMethod("iupHandle", "__newindex", ihandle_settable)
iup.SetMethod("iupHandle", "__tostring", iup.ihandle_tostring) -- implemented in C
iup.SetMethod("iupHandle", "__eq", iup.ihandle_compare) -- implemented in C


------------------------------------------------------------------------------
-- Utilities 
------------------------------------------------------------------------------

function iup.SetHandleName(v)  -- used also by radio and zbox
  local name = iup.GetName(v)
  if not name then
    local autoname = string.format("_IUPLUA_NAME(%s)", tostring(v))
    iup.SetHandle(autoname, v)
    return autoname
  end
  return name
end

function iup.RegisterWidget(ctrl) -- called by all the controls initialization functions
  iup[ctrl.nick] = function(param)
    if (not ctrl.constructor) then 
      error("IUP constructor missing for:" .. ctrl.nick) 
    end
    return ctrl:constructor(param)
  end
end

function iup.RegisterHandle(ih, typename)

  iup.SetClass(ih, "iupHandle")
  
  local widget = iup.GetWidget(ih)
  if not widget then
    local class = iup[string.upper(typename)]
    if not class then
      if (iup.IsContainer(ih)) then
        class = iup.BOX
      else
        class = iup.WIDGET
      end
    end

    local widget = { 
      parent = class, 
      ihandle = ih 
      }
    iup.SetClass(widget, "iupWidget")
    iup.SetWidget(ih, widget)
  end
  
  return ih
end

------------------------------------------------------------------------------
-- Widget class (top class) 
------------------------------------------------------------------------------

iup.WIDGET = {
  callback = {}
}

function iup.WIDGET.constructor(class, param)
  local ih = class:createElement(param)  -- all classes must define createElement
  local widget = { 
    parent = class,
    ihandle = ih
  }
  iup.SetClass(ih, "iupHandle")
  iup.SetClass(widget, "iupWidget")
  iup.SetWidget(ih, widget)
  widget:setAttributes(param)
  return ih
end

function iup.WIDGET.setAttributes(widget, param)
  local ih = widget.ihandle
  for i,v in pairs(param) do 
    if type(i) == "number" and iup.GetClass(v) == "iupHandle" then
      -- We should not set this or other elements (such as iuptext)
      -- will erroneosly inherit it
      rawset(widget, i, v)
    else
      -- this will call settable metamethod
      ih[i] = v
    end
  end
end

function iup.WIDGET.show(ih)
  iup.Show(ih)
end

function iup.WIDGET.hide(ih)
  iup.Hide(ih)
end

function iup.WIDGET.map(ih)
  iup.Map(ih)
end

function iup.WIDGET.unmap(ih)
  iup.Unmap(ih)
end

function iup.WIDGET.detach(ih)
  iup.Detach(ih)
end

function iup.WIDGET.destroy(ih)
  iup.Destroy(ih)
end

-- all the objects in the hierarchy must be "iupWidget"
-- Must repeat this call for every new widget
iup.SetClass(iup.WIDGET, "iupWidget")


------------------------------------------------------------------------------
-- Box class (inherits from WIDGET) 
------------------------------------------------------------------------------

iup.BOX = {
  parent = iup.WIDGET
}

function iup.BOX.setAttributes(widget, param)
  -- iup.Append will be automatically called after createElement
  -- no need to pass elements in the constructor of boxes
  local ih = widget.ihandle
  local n = #param
  for i = 1, n do
    if iup.GetClass(param[i]) == "iupHandle" then 
      iup.Append(ih, param[i]) 
    end
  end
  iup.WIDGET.setAttributes(widget, param)
end

function iup.BOX.append(ih, child)
  return iup.Append(ih, child)
end

function iup.BOX.insert(ih, ref_child, child)
  return iup.Insert(ih, ref_child, child)
end

iup.SetClass(iup.BOX, "iupWidget")

------------------------------------------------------------------------------
-- Compatibility functions.
------------------------------------------------------------------------------

function iup.RGB(r, g, b)
  return string.format("%d %d %d", 255*r, 255*g, 255*b)
end

-- This will allow both names to be used in the same application
-- also will allow static linking to work with require for the main library (only)
if _G.package then
  _G.package.loaded["iuplua"] = iup
  iup._M = iup
  iup._PACKAGE = "iuplua"
end

function iup.layoutdialog(obj)
  return iup.LayoutDialog(obj[1])
end
