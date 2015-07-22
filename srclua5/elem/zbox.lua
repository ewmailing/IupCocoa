------------------------------------------------------------------------------
-- ZBox class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "zbox",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  callback = {}
}

function ctrl.append(ih, child)
  -- automatically add names
  iup.SetHandleName(child)
  iup.Append(ih, child)
end

function ctrl.SetChildrenNames(param)
  if param then
    local i = 1
    while param[i] do
      -- automatically add names
      iup.SetHandleName(param[i])
      i = i+1
    end
  end
end

function ctrl.createElement(class, param)
   ctrl.SetChildrenNames(param)
   return iup.Zbox()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
