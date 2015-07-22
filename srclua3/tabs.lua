IUPTABS = {parent = WIDGET}

function IUPTABS:CreateIUPelement (obj)
  local handle = iupCreateTabs (obj, getn(obj))
  local i = 1
  while obj[i] do
    obj[i].IUP_parent = handle
    i = i + 1
  end
  return handle
end

function iuptabs (o)
  return IUPTABS:Constructor (o)
end
iup.tabs = iuptabs

iup_callbacks.tabchange = {"TABCHANGE_CB", iup_tabchange_cb}
iup_callbacks.tabchange_cb = iup_callbacks.tabchange