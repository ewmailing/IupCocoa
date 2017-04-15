------------------------------------------------------------------------------
-- FlatTabs class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "flattabs",
  parent = iup.BOX,
  subdir = "elem",
  creation = "-",
  funcname = "FlatTabs",
  callback = {
    tabchange_cb = "ii",
    tabchangepos_cb = "nn",
    tabclose_cb = "n",
    --rightclick_cb = "n", -- already implemented in IupTree, same definition
  },
}

function ctrl.createElement(class, param)
  return iup.FlatTabs()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
