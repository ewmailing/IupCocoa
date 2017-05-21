------------------------------------------------------------------------------
-- MglPlot class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "mglplot",
  parent = iup.WIDGET,
  creation = "",
  funcname = "MglPlot",
  subdir = "ctrl",
  callback = {
    predraw_cb = "",
    postdraw_cb = "",
  },
  include = "iup_mglplot.h",
  extrafuncs = 1,
}

function ctrl.createElement(class, param)
   return iup.MglPlot(param.action)
end

ctrl.Begin       = iup.MglPlotBegin
ctrl.Add1D       = iup.MglPlotAdd1D
ctrl.Add2D       = iup.MglPlotAdd2D
ctrl.Add3D       = iup.MglPlotAdd3D
ctrl.End         = iup.MglPlotEnd
ctrl.NewDataSet  = iup.MglPlotNewDataSet
ctrl.Insert1D    = iup.MglPlotInsert1D
ctrl.Insert2D    = iup.MglPlotInsert2D
ctrl.Insert3D    = iup.MglPlotInsert3D
ctrl.Set1D       = iup.MglPlotSet1D
ctrl.Set2D       = iup.MglPlotSet2D
ctrl.Set3D       = iup.MglPlotSet3D
ctrl.SetFormula  = iup.MglPlotSetFormula
ctrl.LoadData    = iup.MglPlotLoadData
ctrl.SetFromFormula  = iup.MglPlotSetFromFormula
ctrl.SetData      = iup.MglPlotSetData
ctrl.Transform    = iup.MglPlotTransform
ctrl.TransformTo = iup.MglPlotTransformTo
ctrl.PaintTo      = iup.MglPlotPaintTo
ctrl.DrawText     = iup.MglPlotDrawText
ctrl.DrawLine     = iup.MglPlotDrawLine
ctrl.DrawMark     = iup.MglPlotDrawMark

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
