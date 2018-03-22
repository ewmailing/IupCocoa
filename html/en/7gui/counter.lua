require("iuplua")

counter = 0

function addCount()
	counter = counter + 1
end

function getCount()
	return counter
end

--********************************** Main *****************************************

txt_count = iup.text{value = getCount(), readonly = "YES",  size = "60"}
btn_count = iup.button{title = "Count", size = "60"}

dlg = iup.dialog{iup.hbox{txt_count, btn_count; ngap = "10"}, title = "Counter", margin = "10x10"}

function btn_count:action()
  addCount()
  txt_count.value = getCount()
end

dlg:showxy( iup.CENTER, iup.CENTER )

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
