IUPCELLS = {parent = WIDGET}

function IUPCELLS:CreateIUPelement( obj )
  return iupCreateCells()
end

function IUPCELLS:redraw()
   self.repaint = IUP_YES 
end

function iupcells(o)
  return IUPCELLS:Constructor(o)
end
iup.cells = iupcells


-- iup_callbacks.draw_cb      = iup_callbacks.draw         = {"DRAW_CB", iup_mat_draw_cb} -- same callback at IupMatrix

iup_callbacks.mouseclick   = {"MOUSECLICK_CB", iup_cells_mouseclick_cb}
iup_callbacks.mousemotion  = {"MOUSEMOTION_CB", iup_cells_mousemotion_cb}
iup_callbacks.scrolling    = {"SCROLLING_CB", iup_cells_scrolling_cb}
iup_callbacks.width        = {"WIDTH_CB", iup_cells_width_cb}
iup_callbacks.height       = {"HEIGHT_CB", iup_cells_height_cb}
iup_callbacks.nlines       = {"NLINES_CB", iup_cells_nlines_cb}
iup_callbacks.ncols        = {"NCOLS_CB", iup_cells_ncols_cb}
iup_callbacks.hspan        = {"HSPAN_CB", iup_cells_hspan_cb}
iup_callbacks.vspan        = {"VSPAN_CB", iup_cells_vspan_cb}

iup_callbacks.mouseclick_cb   = iup_callbacks.mouseclick  
iup_callbacks.mousemotion_cb  = iup_callbacks.mousemotion 
iup_callbacks.scrolling_cb    = iup_callbacks.scrolling   
iup_callbacks.width_cb        = iup_callbacks.width       
iup_callbacks.height_cb       = iup_callbacks.height      
iup_callbacks.nlines_cb       = iup_callbacks.nlines      
iup_callbacks.ncols_cb        = iup_callbacks.ncols       
iup_callbacks.hspan_cb        = iup_callbacks.hspan       
iup_callbacks.vspan_cb        = iup_callbacks.vspan       
