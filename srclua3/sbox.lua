IUPSBOX = {parent = WIDGET}

function IUPSBOX:CreateIUPelement (obj)
  local handle = iupCreateSbox(obj[1])
  if (obj[1]) then obj[1].IUP_parent = handle end
  return handle
end

function iupsbox (o)
  return IUPSBOX:Constructor (o)
end
iup.sbox = iupsbox
