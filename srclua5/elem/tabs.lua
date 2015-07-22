------------------------------------------------------------------------------
-- Tabs class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "tabs",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  callback = {
    tabchange_cb = "ii",
    tabchangepos_cb = "nn",
    tabclose_cb = "n",
    --rightclick_cb = "n", -- already implemented in IupTree, same definition
  },
}

function ctrl.createElement(class, param)
  return iup.Tabs()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
