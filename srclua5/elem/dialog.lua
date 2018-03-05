------------------------------------------------------------------------------
-- Dialog class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "dialog",
  parent = iup.BOX,
  subdir = "elem",
  creation = "I",
  callback = {
    map_cb = "",
    unmap_cb = "",
    destroy_cb = "",
    close_cb = "",
    show_cb = "n",
    move_cb = "nn",
    tips_cb = "nn",
    copydata_cb = "sn",
    trayclick_cb = "nnn",
    dropfiles_cb = "snnn",
    dragbegin_cb = "nn",
    dragdatasize_cb = "s",
    dragdata_cb = "son",
    dragend_cb = "n",
    dropdata_cb = "sonnn",
    customframedraw_cb = "",
    customframeactivate_cb = "n",
  }
}

function ctrl.createElement(class, param)
   return iup.Dialog()
end

function ctrl.popup(ih, x, y)
  iup.Popup(ih,x,y)
end

function ctrl.showxy(ih, x, y)
  return iup.ShowXY(ih, x, y)
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
