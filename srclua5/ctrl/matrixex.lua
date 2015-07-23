------------------------------------------------------------------------------
-- MatrixEx class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "matrixex",
  parent = iup.WIDGET,
  creation = "",
  funcname = "MatrixEx",
  subdir = "ctrl",
  callback = {
    pastesize_cb = "nn",
    busy_cb = "nns",
    numericgetvalue_cb = {"nn", ret = "d"},
    numericsetvalue_cb = "nnd",
    sortcolumncompare_cb = "nnn",
    menucontext_cb = "inn",
    menucontextclose_cb = "inn",
  },
  include = "iupmatrixex.h",
  extrafuncs = 1,
  openfuncname = "_matrixex",
}

function ctrl.createElement(class, param)
   return iup.MatrixEx(param.action)
end

function ctrl.setcell(ih, l, c, val)
  iup.SetAttributeId2(ih,"",l,c,val)
end

function ctrl.getcell(ih, l, c)
  return iup.GetAttributeId2(ih,"",l,c)
end

-- Defined in C
ctrl.SetFormula = iup.MatrixSetFormula
ctrl.SetDynamic = iup.MatrixSetDynamic

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
