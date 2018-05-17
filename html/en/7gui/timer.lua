require("iuplua")

--********************************** Main *****************************************

timer = iup.timer{time = 100, run = "YES", totalelapsedtime = 0}
glabel = iup.label{title = "Elapsed Time:"}
gauge = iup.gauge{min = .1, max = 30, expand = "HORIZONATAL", showtext="NO"}
label = iup.label{text = "0.0s", expand = "HORIZONTAL"}
vlable = iup.label{title = "Duration:"}
val = iup.val{"HORIZONTAL"; expand = "HORIZONTAL", min = "0", max = "60", step = ".1", value = "30.0", pagestep = ".5"}
button = iup.button{title = "Reset", expand = "HORIZONTAL"}

hbox1 = iup.hbox{glabel, gauge; margin = "0x0", alignment = "ACENTER"}
hbox2 = iup.hbox{vlable, val; margin = "0x0", alignment = "ACENTER"}
vbox = iup.vbox{hbox1, label, hbox2, button; margin = "10x10", gap = 10}

dlg = iup.dialog{vbox, title = "Timer", size = "200"}

function timer:action_cb()

  local elapsedTime = self.elapsedtime or 0
  local totaltime = self.totalelapsedtime
  
  local t = (totaltime + elapsedTime) / 1000.
  local valValue = val.value
  
  if t >= tonumber(valValue) then
    self.run = "NO"
    self.totalelapsedtime =  totaltime + elapsedTime
    label.title = string.format("%.2fs", val.value)
    gauge.value = val.value
  else
    label.title = string.format("%.2fs", (totaltime + elapsedTime) / 1000.)
    gauge.value = (totaltime + elapsedTime) / 1000.
	print(gauge.value.." - "..type(gauge.value))
  end
end

function val:valuechanged_cb()

  local totalTime = timer.totalelapsedtime

  gauge.max = self.value

  if totalTime/1000. < tonumber(self.value) then
    timer.run = "YES"
  else
    gauge.value = gauge.max
  end
end

function button:action()

  timer.run = "NO"

  timer.totalelapsedtime = 0
  val.value = "30."
  gauge.value = 0
  gauge.min = 0
  gauge.max = 60

  timer.run = "YES"
end

dlg:showxy( iup.CENTER, iup.CENTER )

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
