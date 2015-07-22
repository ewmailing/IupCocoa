IUPVAL = {parent = WIDGET}

function IUPVAL:CreateIUPelement (obj)
  return iupCreateVal (obj[1])
end

function iupval (o)
  return IUPVAL:Constructor (o)
end
iup.val = iupval


-- must set here because it is also used elsewhere with a different signature
iup_callbacks.mousemove = {"MOUSEMOVE_CB", nil}
iup_callbacks.mousemove_cb = iup_callbacks.mousemove
iup_callbacks.mousemove.val  = iup_val_mousemove_cb

iup_callbacks.buttonpress    = {"BUTTON_PRESS_CB", iup_val_button_press_cb}
iup_callbacks.buttonrelease  = {"BUTTON_RELEASE_CB", iup_val_button_release_cb}

iup_callbacks.button_press_cb    = iup_callbacks.buttonpress   
iup_callbacks.button_release_cb  = iup_callbacks.buttonrelease 
