IUPDIAL = {parent = WIDGET}

function IUPDIAL:CreateIUPelement (obj)
  return iupCreateDial (obj[1])
end

function iupdial (o)
  return IUPDIAL:Constructor (o)
end
iup.dial = iupdial

iup_callbacks.mousemove.dial = iup_val_mousemove_cb  -- same callback at IupVal, and conflict with IupMatrix

-- iup_callbacks.buttonpress  = {"BUTTON_PRESS_CB", iup_val_button_press_cb}  -- same callback at IupVal
-- iup_callbacks.buttonrelease  = {"BUTTON_RELEASE_CB", iup_val_button_release_cb} -- same callback at IupVal
