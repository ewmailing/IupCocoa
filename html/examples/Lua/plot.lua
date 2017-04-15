
require("iuplua")
require("iuplua_plot")

plot = iup.plot{
--    TITLE = "Plot Test",
    MARGINBOTTOM = 20,
}

--iup.PlotBegin(plot, 0)
--iup.PlotAdd(plot, 0, 0)
--iup.PlotAdd(plot, 1, 1)
--iup.PlotEnd(plot)
plot:Begin(0)
plot:Add(0, 0)
plot:Add(1, 1)
plot:End()

dlg = iup.dialog{
    TITLE = "Plot Test",
    SIZE = "400x200",
    plot,
}

dlg:showxy(iup.CENTER, iup.CENTER)

iup.MainLoop()
