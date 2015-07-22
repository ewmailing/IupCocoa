------------------------------------------------------------------------------
-- Config class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "config",
  parent = iup.WIDGET,
  creation = "",
  funcname = "Config",
  callback = {
    recent_cb = "",
  },
  include = "iup_config.h",
  extrafuncs = 1,
}

ctrl.SetVariable = iup.ConfigSetVariable
ctrl.SetVariableId = iup.ConfigSetVariableId
ctrl.GetVariable = iup.ConfigGetVariable
ctrl.GetVariableId = iup.ConfigGetVariableId
ctrl.GetVariableDef = iup.ConfigGetVariableDef
ctrl.GetVariableIdDef = iup.ConfigGetVariableIdDef
ctrl.Load = iup.ConfigLoad
ctrl.Save = iup.ConfigSave
ctrl.RecentInit = iup.ConfigRecentInit
ctrl.RecentUpdate = iup.ConfigRecentUpdate
ctrl.DialogShow = iup.ConfigDialogShow
ctrl.DialogClosed = iup.ConfigDialogClosed

function ctrl.createElement(class, param)
   return iup.Config()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
