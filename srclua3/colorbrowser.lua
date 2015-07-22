IUPCOLORBROWSER = {parent = WIDGET}

function IUPCOLORBROWSER:CreateIUPelement(obj)
  return iupCreateColorBrowser(obj)
end

function iupcolorbrowser (o)
  return IUPCOLORBROWSER:Constructor (o)
end
iup.colorbrowser = iupcolorbrowser


iup_callbacks.drag   = {"DRAG_CB", iup_colorbrowser_drag_cb}
iup_callbacks.change = {"CHANGE_CB", iup_colorbrowser_change_cb}

iup_callbacks.drag_cb   = iup_callbacks.drag  
iup_callbacks.change_cb = iup_callbacks.change
