IUPSPLIT = {parent = WIDGET}

function IUPSPLIT:CreateIUPelement (obj)
  local handle = iupCreateSplit(obj[1], obj[2])
  if (obj[1]) then obj[1].IUP_parent = handle end
  if (obj[2]) then obj[2].IUP_parent = handle end
  return handle
end

function iupsplit (o)
  return IUPSPLIT:Constructor (o)
end
iup.split = iupsplit
