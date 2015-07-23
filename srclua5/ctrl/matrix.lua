------------------------------------------------------------------------------
-- Matrix class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "matrix",
  parent = iup.WIDGET,
  creation = "-",
  subdir = "ctrl",
  callback = {
    action_cb = "nnnns",
    click_cb = "nns",
    release_cb = "nns",
    mousemove_cb = "nn",
    editclick_cb = "nns",
    editrelease_cb = "nns",
    editmousemove_cb = "nn",
    drop_cb = "inn",
    menudrop_cb = "inn",
    dropcheck_cb = "nn",
    draw_cb = "nnnnnnn",  -- fake definitions to be replaced by matrixfuncs module
    dropselect_cb = "nnisnn",
    edition_cb = "nnnn",
    enteritem_cb = "nn",
    leaveitem_cb = "nn",
    togglevalue_cb = "nnn",
    colresize_cb = "n",
    scrolltop_cb = "nn",
    fgcolor_cb = "nn",  -- fake definitions to be replaced by matrixfuncs module
    bgcolor_cb = "nn",  -- fake definitions to be replaced by matrixfuncs module
    font_cb = {"nn", ret = "s"}, -- ret is return type
    type_cb = {"nn", ret = "s"}, -- ret is return type
    value_cb = {"nn", ret = "s"}, -- ret is return type
    value_edit_cb = "nns",
    translatevalue_cb = {"nns", ret = "s"}, -- ret is return type
    mark_cb = "nn",
    markedit_cb = "nnn",
    resizematrix_cb = "nn",
  },
  include = "iupcontrols.h",
  extrafuncs = 1,
}

function ctrl.createElement(class, param)
   return iup.Matrix(param.action)
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
