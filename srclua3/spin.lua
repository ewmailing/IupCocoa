IUPSPIN = {parent = WIDGET}

function IUPSPIN:CreateIUPelement ()
  return iupCreateSpin ()
end

function iupspin (o)
  return IUPSPIN:Constructor (o)
end
iup.spin = iupspin

IUPSPINBOX = {parent = WIDGET}

function IUPSPINBOX:CreateIUPelement (obj)
  return iupCreateSpinbox (obj[1])
end

function iupspinbox (o)
  return IUPSPINBOX:Constructor (o)
end
iup.spinbox = iupspinbox

iup_callbacks.spincb = {"SPIN_CB", iup_spin_cb}
iup_callbacks.spin_cb = iup_callbacks.spincb
