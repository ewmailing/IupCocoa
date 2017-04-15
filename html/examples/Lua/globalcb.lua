
function gbutton_cb(but, press, x, y, status)
  print(but, press, x, y, status)
end

function gkeypress_cb(code, pressed)
  print(code, pressed)
end

iup.SetGlobal("INPUTCALLBACKS", "Yes")
iup.SetGlobalCallback("GLOBALBUTTON_CB", gbutton_cb)
iup.SetGlobalCallback("GLOBALKEYPRESS_CB", gkeypress_cb)

