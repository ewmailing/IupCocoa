------------------------------------------------------------------------------
-- OleControl class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "olecontrol",
  parent = iup.WIDGET,
  creation = "s",
  funcname = "OleControl",
  subdir = "ctrl",
  callback = {},
  include = "iupole.h",
  extracode = [[ 
int iupolelua_open(lua_State* L)
{
  if (iuplua_opencall_internal(L))
    IupOleControlOpen();
    
  iuplua_get_env(L);
  iupolecontrollua_open(L);
  return 0;
}

/* obligatory to use require"iupluaole" */
int luaopen_iupluaole(lua_State* L)
{
  return iupolelua_open(L);
}

]]
}

function ctrl.createElement(class, param)
  return iup.OleControl(param[1])
end

function ctrl.CreateLuaCOM(ih)
  -- if luacom is loaded, use it to access methods and properties
  -- of the control
  if luacom then
    local punk = ih.iunknown
    if punk then
      ih.com = luacom.CreateLuaCOM(luacom.ImportIUnknown(punk))
    end     
  end
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
