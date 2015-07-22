

-- #################################################################################
--                                 Private functions
-- #################################################################################

-- maps Ihandles into Lua objects
iup_handles = {}

settagmethod(iuplua_tag, "gettable", iup_gettable) 
settagmethod(iuplua_tag, "settable", iup_settable)
settagmethod (tag({}), "index", iup_index)

function _ALERT(s)
  local bt = iupbutton{title="Ok", size="60", action="return IUP_CLOSE"}
  local ml = iupmultiline{expand="YES", readonly="YES", value=s, size="300x150"}
  local vb = iupvbox{ml, bt; alignment="ACENTER", margin="10x10", gap="10"}
  local dg = iupdialog{vb; title="Lua Error",defaultesc=bt,defaultenter=bt,startfocus=bt}
  dg:popup(IUP_CENTER, IUP_CENTER)
  dg:destroy()
end

function type_string (o) 
  return type(o) == "string" 
end

function type_number (o) 
  return type(o) == "number" 
end

function type_nil (o)    
  return type(o) == "nil" 
end

function type_function (o)
  return type(o) == "function" 
end

function type_widget(w)
  if w then
    return iup_handles[w]
  else
    return nil
  end
end

function type_menu (o) 
  return type_widget(o) and (o.parent==IUPMENU) 
end

function type_item (o)
  return type_widget(o) and (o.parent==IUPITEM or o.parent==IUPSUBMENU or o.parent==IUPSEPARATOR)
end

function iupCallMethod(name, ...)
  local handle = arg[1] -- always the handle
 
  local func = handle[name] -- this is the old name
  if (not func) then
    local full_name = strlower(iup_callbacks[name][1])
    func = handle[full_name]  -- check also for the full name
    
    if (not func) then
      return
    end
  end
    
  if type_function (func) then
    return call(func, arg)
  elseif type_string(func) then
    local temp = self
    self = handle
    local result = dostring(func)
    self = temp
    return result
  else
    return IUP_ERROR
  end
end

function iupSetName (handle)
  if not type_string(iup_handles[handle].IUP_name) then
    iup_handles[handle].IUP_name = format("_IUPLUA_NAME(%s)", tostring(handle))
    IupSetHandle(handle.IUP_name, handle)
  end
end

function iupCreateChildrenNames (obj)
  if obj.parent.parent == COMPOSITION then
    local i = 1
    while obj[i] do
      iupCreateChildrenNames (obj[i])
      i = i+1
    end
  elseif obj.parent == IUPFRAME then
    iupCreateChildrenNames (obj[1])
  else
    iupSetName (obj)
  end
end


-- #################################################################################
--                              Public Functions
-- #################################################################################


function IupRGB (red, green, blue)
  return floor(red*255).." "..floor(green*255).." "..floor(blue*255)
end
iup.RGB = IupRGB

function IupRegisterHandle(handle, typename)
  if not iup_handles[handle] then
    local obj = getglobal("IUP"..strupper(typename))
    if not obj then
      obj = WIDGET
    end
    iup_handles[handle] = { parent=obj, handle=handle }
  end
  return handle
end
iup.RegisterHandle = IupRegisterHandle

function IupGetFromC(obj)
  local handle = IupGetHandle(obj[1])
  return IupRegisterHandle(handle, IupGetClassName(handle))
end

iup.GetFromC = function (name)
  local handle = IupGetHandle(name)
  return IupRegisterHandle(handle, IupGetClassName(handle))
end

