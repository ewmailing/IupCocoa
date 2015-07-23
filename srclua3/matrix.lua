IUPMATRIX = {parent = WIDGET}

function IUPMATRIX:CreateIUPelement (obj)
  return iupCreateMatrix ()
end

function IUPMATRIX:setcell(l,c,val)
   IupSetAttribute(self,l..":"..c,val)
end

function IUPMATRIX:getcell(l,c,val)
   return IupGetAttribute(self,l..":"..c)
end

function iupmatrix (o)
  return IUPMATRIX:Constructor (o)
end
iup.matrix = iupmatrix


iup_callbacks.actioncb.matrix = iup_mat_action_cb
iup_callbacks.mousemove.matrix = iup_mat_mousemove_cb

iup_callbacks.edition     = {"EDITION_CB", iup_mat_edition_cb}
iup_callbacks.drop        = {"DROP_CB", iup_mat_drop_cb}
iup_callbacks.dropselect  = {"DROPSELECT_CB", iup_mat_dropselect_cb}
iup_callbacks.enteritem   = {"ENTERITEM_CB", iup_mat_enteritem_cb}
iup_callbacks.leaveitem   = {"LEAVEITEM_CB", iup_mat_leaveitem_cb}
iup_callbacks.click       = {"CLICK_CB", iup_mat_click_cb}
iup_callbacks.scrolltop   = {"SCROLLTOP_CB", iup_mat_scrolltop_cb}
iup_callbacks.valuecb     = {"VALUE_CB", iup_mat_value_cb}
iup_callbacks.draw        = {"DRAW_CB", iup_mat_draw_cb}
iup_callbacks.dropcheck   = {"DROPCHECK_CB", iup_mat_dropcheck_cb}
iup_callbacks.fgcolorcb   = {"FGCOLOR_CB", iup_mat_fgcolor_cb}
iup_callbacks.bgcolorcb   = {"BGCOLOR_CB", iup_mat_bgcolor_cb}
iup_callbacks.value_edit  = {"VALUE_EDIT_CB", iup_mat_value_edit_cb}
iup_callbacks.markedit_cb = {"MARKEDIT_CB", iup_mat_markedit_cb}
iup_callbacks.mark_cb     = {"MARK_CB", iup_mat_mark_cb}
iup_callbacks.mouse_cb    = {"MOUSE_CB", iup_mat_mouse_cb}
iup_callbacks.font_cb     = {"FONT_CB", iup_mat_font_cb}

iup_callbacks.edition_cb    = iup_callbacks.edition    
iup_callbacks.drop_cb       = iup_callbacks.drop       
iup_callbacks.dropselect_cb = iup_callbacks.dropselect 
iup_callbacks.enteritem_cb  = iup_callbacks.enteritem  
iup_callbacks.leaveitem_cb  = iup_callbacks.leaveitem  
iup_callbacks.click_cb      = iup_callbacks.click      
iup_callbacks.scrolltop_cb  = iup_callbacks.scrolltop  
iup_callbacks.value_cb      = iup_callbacks.valuecb    
iup_callbacks.draw_cb       = iup_callbacks.draw       
iup_callbacks.dropcheck_cb  = iup_callbacks.dropcheck  
iup_callbacks.fgcolor_cb    = iup_callbacks.fgcolorcb  
iup_callbacks.bgcolor_cb    = iup_callbacks.bgcolorcb  
iup_callbacks.value_edit_cb = iup_callbacks.value_edit 
