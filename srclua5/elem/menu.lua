------------------------------------------------------------------------------
-- Menu class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "menu",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  callback = {
    open_cb = "",
    menuclose_cb = "",
  }
}

function ctrl.popup(ih, x, y)
  iup.Popup(ih, x, y)
end

function ctrl.getargs(menu_param)
  local itemarg = {}
  -- copy the named attributes to the element parameters
  for u,v in pairs(menu_param) do
    if type(u) ~= "number" then
      itemarg[u] = v
    end
  end
  return itemarg
end

function ctrl.createElement(class, param)
  local n = #param
  for i=1,n do
    local menu_param = param[i]
    if type(menu_param) == "table" then 
      -- replace param[i], so it will be used by iup.Append after createElement in setAttributes
      -- other elements already created can also be used
      if type(menu_param[1]) == "nil" then
        param[i] = iup.separator{}
      elseif type(menu_param[1]) == "string" then
        local itemarg = ctrl.getargs(menu_param)
        if type(menu_param[2]) == "userdata" then
          itemarg[1] = menu_param[2]
          itemarg.title = menu_param[1]
          param[i] = iup.submenu(itemarg)
        else
          itemarg.title = menu_param[1]
          itemarg.action = menu_param[2]
          param[i] = iup.item(itemarg)
        end
      end
    end
  end
  return iup.Menu()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
