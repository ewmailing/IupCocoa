IUPSCROLLBOX = {parent = WIDGET}

function IUPSCROLLBOX:CreateIUPelement (obj)
  local handle = iupCreateScrollBox(obj[1])
  if (obj[1]) then obj[1].IUP_parent = handle end
  return handle
end

function iupscrollbox (o)
  return IUPSCROLLBOX:Constructor (o)
end
iup.scrollbox = iupscrollbox
