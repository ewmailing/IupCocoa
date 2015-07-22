IUPCOLORBAR = {parent = WIDGET}

function IUPCOLORBAR:CreateIUPelement(obj)
  return iupCreateColorbar(obj)
end

function iupcolorbar (o)
  return IUPCOLORBAR:Constructor (o)
end
iup.colorbar = iupcolorbar


iup_callbacks.cellcb      = {"CELL_CB", iup_colorbar_cell_cb}
iup_callbacks.selectcb    = {"SELECT_CB", iup_colorbar_select_cb}
iup_callbacks.switchcb    = {"SWITCH_CB", iup_colorbar_switch_cb}
iup_callbacks.extendedcb  = {"EXTENDED_CB", iup_colorbar_extended_cb}

iup_callbacks.cell_cb      = iup_callbacks.cellcb      
iup_callbacks.select_cb    = iup_callbacks.selectcb    
iup_callbacks.switch_cb    = iup_callbacks.switchcb    
iup_callbacks.extended_cb  = iup_callbacks.extendedcb  
