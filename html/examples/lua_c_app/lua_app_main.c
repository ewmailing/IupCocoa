
/*
The purpose of this file is to start up the program and get into Lua.
Because accessing the resource directory to load script files is problematic on some platforms, 
you are expected to modify this file as needed to get your script to load.
Modify LoadLuaScript() to suit your needs.
bin2c.lua can generate a C header file which you can directly #include to compile in your script without needing to load anything from the file system.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "iup.h"
#include "iupcbs.h"
#include "iupweb.h"
#include "iuplua.h"
#include "iupluaweb.h"



struct GlobalResources
{
	struct lua_State* luaState;
};

struct GlobalResources* g_globalResources = NULL;
static _Bool s_isInit = 0;


// Modify this function to suit your needs.
static void LoadLuaScript()
{
	struct lua_State* L;

	L = luaL_newstate();
	g_globalResources->luaState = L;

	luaL_openlibs(L);
	iuplua_open(L);
	iupweblua_open(L);

	// Start modifying here. bin2c.lua can make this simple.
	#include "testscript.h"

}



void DoStartupOnlyOnce(void);
void IupEntryPoint(void);

// Do not modify main(). Not all platforms use main as the entry point so your changes may have no effect.
int main(int argc, char* argv[])
{
	// Remember: Not all platforms run main.
	DoStartupOnlyOnce();
	IupOpen(&argc, &argv); // removed because IupLua is presumably doing this.
	IupSetFunction("ENTRY_POINT", (Icallback)IupEntryPoint);
	IupMainLoop();
	return 0;
}

static int report(lua_State* L, int status)
{
	const char* msg;
	if(status)
	{
		msg = lua_tostring(L, -1);
		if (msg == NULL) msg = "(error with no message)";
		IupLog("ERROR", "status=%d, %s\n", status, msg);
		lua_pop(L, 1);
	}
	return status;
}


static void CallLuaVoidFunction(struct lua_State* lua_state, const char* function_name)
{
	lua_getglobal(lua_state, function_name);
	int ret_flag = lua_pcall(lua_state, 0, 0, 0);
	if(0 != ret_flag)
	{
		report(lua_state, ret_flag);
	}
	else
	{
		// nothing to lua_pop with void function
	}
}

static void DestroyGlobals()
{
    free(g_globalResources);
    g_globalResources = NULL;
}

// This function is called when the program exits.
static void IupExitPoint()
{
	CallLuaVoidFunction(g_globalResources->luaState, "IupExitPoint");

//	iuplua_close(g_globalResources->luaState);
	lua_close(g_globalResources->luaState);
	g_globalResources->luaState = NULL;
//	IupClose();
	s_isInit = 0;
	
	DestroyGlobals();
}

// IupLua has a strange startup sequence that is causing problems with the next-gen Iup backends because all of them require deferring to the OS's native event loop.
// IupLua seems to check to see if IupOpen is called outside of Lua first or in Lua and will do different things.
// When called outside of Lua, it skips certain registration operations which is breaking us.
// iuplua_opencall_internal in particular returns false in this case, which skips calling IupWebBrowserOpen in iupweb and breaks things.
// Unfortunately, because we must defer to the OS native loop for the new platforms,
// we are relying on the new IupEntryPoint behavior which means we must start in C and use Iup.
// The workaroud seems to be to immediately start Lua and call require() to get things going.
// Because the next-gen startup is not guaranteed to use main as the sole entry point, we must take care to not do our initialization twice.
// See _IUPOPEN_CALL and EXTERNAL and INTERNAL and iuplua_opencall_internal and iupweblua_open
void DoStartupOnlyOnce()
{
	if(s_isInit == 1)
	{
		return;
	}
	
	g_globalResources = calloc(1, sizeof(struct GlobalResources));

	// For some reason, I must call this because my attempts to use IupLuaWeb without this result in this never being called.
	int error_val = IupWebBrowserOpen();
	// Some platforms may not provide IupWebBrowser
	if(IUP_ERROR == error_val)
	{
		// We'll pop up a message dialog alerting of the problem.
		Ihandle* msg_dlg = IupMessageDlg();

		IupSetAttribute(msg_dlg, "DIALOGTYPE", "ERROR");
		IupSetAttribute(msg_dlg, "TITLE", "IupWeb failed to initialize");
		IupSetAttribute(msg_dlg, "BUTTONS", "1");

		const char* missing_library_name = IupGetGlobal("_IUP_WEBBROWSER_MISSING_DLL");
		if(NULL != missing_library_name)
		{
			IupSetStrf(msg_dlg, "VALUE", "Unable to load dependency %s. Please install this via your system's package manager and try again.", missing_library_name);
		}
		else
		{
			IupSetAttribute(msg_dlg, "VALUE", "You may need to install dependencies to support IupWeb.");
		}

		IupPopup(msg_dlg, IUP_CURRENT, IUP_CURRENT);
		IupDestroy(msg_dlg);
	}
	else
	{
	}


	LoadLuaScript();


	s_isInit = 1;
}

void IupEntryPoint()
{
	// most platforms have started in main(), but not all of them.
	DoStartupOnlyOnce();

	// This tells IUP to call IupExitPoint on exit.
	IupSetFunction("EXIT_CB", (Icallback)IupExitPoint);
    CallLuaVoidFunction(g_globalResources->luaState, "IupEntryPoint");
	IupSetInt(NULL, "UTF8MODE", 1);

}


