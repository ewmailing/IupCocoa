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
    --tabchange_cb = "ii", -- already implemented in IupTabs, same definition
    --tabchangepos_cb = "nn", -- already implemented in IupTabs, same definition
    --tabclose_cb = "n", -- already implemented in IupTabs, same definition
    --extrabutton_cb = "nn", -- already implemented in IupExpander, same definition
    --rightclick_cb = "n", -- already implemented in IupTree, same definition
  },
}

function ctrl.createElement(class, param)
  return iup.FlatTabs()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
