------------------------------------------------------------------------------
-- LuaScripterDlg class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "luascripterdlg",
  parent = iup.WIDGET,
  subdir = "scripter",
  creation = "",
  callback = {
  },
  include = "iupluascripterdlg.h",
  funcname = "LuaScripterDlg",
  openfuncname = "_luascripterdlg",
  extracode = [[ 
int iupluascripterdlglua_open(lua_State* L)
{
  if (iuplua_opencall_internal(L))
    IupLuaScripterDlgOpen();
    
  iuplua_get_env(L);
  iup_luascripterdlglua_open(L);
  return 0;
}

/* obligatory to use require"iupluascripterdlg" */
int luaopen_iupluascripterdlg(lua_State* L)
{
  return iupluascripterdlglua_open(L);
}

]]
} 

function ctrl.showxy(ih, x, y)
  return iup.ShowXY(ih, x, y)
end

function ctrl.popup(ih, x, y)
  iup.Popup(ih,x,y)
end

function ctrl.createElement(class, param)
   return iup.LuaScripterDlg()
end
   
iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
