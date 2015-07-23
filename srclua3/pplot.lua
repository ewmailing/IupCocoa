IUPPPLOT = {parent = WIDGET}

function IUPPPLOT:CreateIUPelement (obj)
  return iupCreatePPlot ()
end

function iuppplot (o)
  return IUPPPLOT:Constructor (o)
end
iup.pplot = iuppplot

iup_callbacks.edit_cb.pplot  = iup_pplot_edit_cb

iup_callbacks.editbegin_cb   = {"EDITBEGIN_CB", iup_pplot_editbegin_cb}
iup_callbacks.editend_cb     = {"EDITEND_CB", iup_pplot_editend_cb}
iup_callbacks.select_cb      = {"SELECT_CB", iup_pplot_select_cb}
iup_callbacks.selectbegin_cb = {"SELECTBEGIN_CB", iup_pplot_selectbegin_cb}
iup_callbacks.selectend_cb   = {"SELECTEND_CB", iup_pplot_selectend_cb}
iup_callbacks.delete_cb      = {"DELETE_CB", iup_pplot_delete_cb}
iup_callbacks.deletebegin_cb = {"DELETEBEGIN_CB", iup_pplot_deletebegin_cb}
iup_callbacks.deleteend_cb   = {"DELETEEND_CB", iup_pplot_deleteend_cb}
iup_callbacks.predraw_cb     = {"PREDRAW_CB", iup_pplot_predraw_cb}
iup_callbacks.postdraw_cb    = {"POSTDRAW_CB", iup_pplot_postdraw_cb}
