IUPGAUGE = {parent = WIDGET}

function IUPGAUGE:CreateIUPelement (obj)
  return iupCreateGauge ()
end

function iupgauge (o)
  return IUPGAUGE:Constructor (o)
end
iup.gauge = iupgauge
