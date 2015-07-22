/** \file
 * \brief Bindig of iuptree to Lua 3.
 *
 * See Copyright Notice in "iup.h"
 */

#include <stdlib.h>

#include "iup.h"
#include "iupcontrols.h"

#include <lua.h>
#include <lauxlib.h>

#include "iuplua.h"
#include "il.h"
#include "il_controls.h"


static int TREE_pattern (Ihandle *handle, int id, char* name)
{
  iuplua_call_start(handle, name);
  lua_pushnumber(id);
  return iuplua_call();
}

static int TREE_branchopen (Ihandle *handle, int id)
{
  return TREE_pattern(handle,id,"branchopen");
}

static int TREE_branchclose (Ihandle *handle, int id)
{
  return TREE_pattern(handle,id,"branchclose");
}

static int TREE_showrename (Ihandle *handle, int id)
{
  return TREE_pattern(handle,id,"showrenamecb");
}

static int TREE_executeleaf (Ihandle *handle, int id)
{
  return TREE_pattern(handle,id,"executeleaf");
}

static int TREE_selection (Ihandle *handle, int node, int state)
{
  iuplua_call_start(handle, "selection");
  lua_pushnumber(node);
  lua_pushnumber(state);
  return iuplua_call();
}

static int TREE_renamenode (Ihandle *handle, int id, char* name)
{
  iuplua_call_start(handle, "renamenode");
  lua_pushnumber(id);
  lua_pushstring(name);
  return iuplua_call();
}

static int TREE_rename (Ihandle *handle, int id, char* name)
{
  iuplua_call_start(handle, "renamecb");
  lua_pushnumber(id);
  lua_pushstring(name);
  return iuplua_call();
}

static int TREE_multiselection (Ihandle *handle, int *ids, int n)
{
  int i;
  lua_Object tb;

  iuplua_call_start(handle, "multiselection");

  tb = lua_createtable();
  for (i = 0; i < n; i++) 
  {
    lua_beginblock();
    lua_pushobject(tb);
    lua_pushnumber(i+1);
    lua_pushnumber(ids[i]);
    lua_settable();
    lua_endblock();   
  }
  lua_pushobject(tb);

  lua_pushnumber (n);
  return iuplua_call();
}

static int TREE_multiunselection (Ihandle *handle, int *ids, int n)
{
  int i;
  lua_Object tb;

  iuplua_call_start(handle, "multiunselection");

  tb = lua_createtable();
  for (i = 0; i < n; i++) 
  {
    lua_beginblock();
    lua_pushobject(tb);
    lua_pushnumber(i+1);
    lua_pushnumber(ids[i]);
    lua_settable();
    lua_endblock();   
  }
  lua_pushobject(tb);

  lua_pushnumber (n);
  return iuplua_call();
}

static int TREE_dragdrop(Ihandle* handle, int drag_id, int drop_id, int isshift, int iscontrol)
{
  iuplua_call_start(handle, "dragdrop");
  lua_pushnumber(drag_id);
  lua_pushnumber(drop_id);
  lua_pushnumber(isshift);
  lua_pushnumber(iscontrol);
  return iuplua_call();
}

static int TREE_rightclick (Ihandle *handle, int id, char* r)
{
  iuplua_call_start(handle, "rightclick");
  lua_pushnumber(id);
  lua_pushstring(r);
  return iuplua_call();
}

static void CreateTree(void)
{
  int tag = (int)lua_getnumber(lua_getglobal("iuplua_tag"));
  lua_pushusertag(IupTree(), tag);
}

static int tree_getnumberintable(lua_Object o)
{
	lua_pushobject(lua_getglobal("IUPTREEREFERENCETABLE"));
	lua_pushobject(o);
	return (int)lua_getnumber(lua_rawgettable());
}

static void tree_setnumberintable(lua_Object o, int ref)
{
	/* Set in global table in index o, the value of ref. 
	   If ref is 0, then set nil in the table. */
	lua_pushobject(lua_getglobal("IUPTREEREFERENCETABLE"));
	lua_pushobject(o);
	if(ref == 0)
		lua_pushnil();
	else
	  lua_pushnumber(ref);
	lua_rawsettable();
}

static void* check_userdata(int numArg)
{
  lua_Object o = lua_getparam(numArg);
  luaL_arg_check(lua_isuserdata(o), numArg, "userdata expected");
  return lua_getuserdata(o);
}

static void cfTreeSetUserId(void)
{
  lua_pushnumber(IupTreeSetUserId(iuplua_checkihandle(1), luaL_check_int(2), check_userdata(3)));
} 

static void cfTreeGetUserId(void)
{
  lua_pushusertag(IupTreeGetUserId(iuplua_checkihandle(1), luaL_check_int(2)), LUA_ANYTAG);
}

static lua_Object tree_gettable(Ihandle* ih, int id)
{
  int ref = (int)IupTreeGetUserId(ih, id);
  if(ref == 0)
    return LUA_NOOBJECT;
	else
		return lua_getref(ref-1);
}

static void cfTreeSetTableId(void)
{
  Ihandle* ih = iuplua_checkihandle(1);
  int id = luaL_check_int(2);

  lua_Object o3 = lua_getparam(3);
  if(lua_isnil(o3))
  {
		int ref;
		/* Setting in the id table the reference as nil */
		o3 = tree_gettable(ih, id);
		if(o3 != LUA_NOOBJECT)
		  tree_setnumberintable(o3, 0);

    ref = (int)IupTreeGetUserId(ih, id);
    lua_unref(ref);
    lua_pushnumber(IupTreeSetUserId(ih, id, NULL));
  }
  else
  {
    int ref;
    lua_pushobject(o3);
    ref = lua_ref(1);
    lua_pushnumber(IupTreeSetUserId(ih, id, (void*)(ref+1)));
		tree_setnumberintable(o3, ref+1);
  }
} 

static void cfTreeGetTable(void)
{
  Ihandle* ih = iuplua_checkihandle(1);
  int id = luaL_check_int(2);

	lua_Object o3 = tree_gettable(ih, id);
  if(o3 == LUA_NOOBJECT)
    lua_pushnil();
  else
    lua_pushobject(o3);
}

static int tree_gettableid(Ihandle* ih, lua_Object o)
{
  int ref = tree_getnumberintable(o);
  if(ref == 0)
		return -1;
  return IupTreeGetId(ih, (void*) ref);
}

static void cfTreeGetTableId(void)
{
  Ihandle* ih = iuplua_checkihandle(1);
  lua_Object o2 = luaL_tablearg(2);
  int ret = tree_gettableid(ih, o2);
	if(ret == -1)
		lua_pushnil();
	else
		lua_pushnumber(ret);
}

static void cfTreeGetId(void)
{
  lua_pushnumber(IupTreeGetId(iuplua_checkihandle(1), check_userdata(2)));
}

int treelua_open(void)
{
  struct AssocList {
    char *name;
    lua_CFunction func;
  } TreeAssocList [] = {
    {"iup_tree_multiselection_cb", (lua_CFunction)TREE_multiselection},
    {"iup_tree_multiunselection_cb", (lua_CFunction)TREE_multiunselection},
    {"iup_tree_selection_cb", (lua_CFunction)TREE_selection},
    {"iup_tree_branchopen_cb", (lua_CFunction)TREE_branchopen},
    {"iup_tree_branchclose_cb", (lua_CFunction)TREE_branchclose},
    {"iup_tree_executeleaf_cb", (lua_CFunction)TREE_executeleaf},
    {"iup_tree_renamenode_cb", (lua_CFunction)TREE_renamenode},
    {"iup_tree_rename_cb", (lua_CFunction)TREE_rename},
    {"iup_tree_showrename_cb", (lua_CFunction)TREE_showrename},
    {"iup_tree_dragdrop_cb", (lua_CFunction)TREE_dragdrop},
    {"iup_tree_rightclick_cb", (lua_CFunction)TREE_rightclick}
  };
  int SizeTreeAssocList = (sizeof(TreeAssocList)/sizeof(struct AssocList));
  int i ;

  iuplua_register("IupTreeSetUserId", cfTreeSetUserId);
  iuplua_register("IupTreeGetUserId", cfTreeGetUserId);
  iuplua_register("IupTreeGetId",     cfTreeGetId);
  iuplua_register("IupTreeSetTableId", cfTreeSetTableId);
  iuplua_register("IupTreeGetTableId", cfTreeGetTableId);
  iuplua_register("IupTreeGetTable", cfTreeGetTable);

  lua_register("iupCreateTree", CreateTree);

  for (i = 0; i < SizeTreeAssocList; i++)
     lua_register(TreeAssocList[i].name, TreeAssocList[i].func);

#ifdef IUPLUA_USELOH
#ifdef TEC_BIGENDIAN
#ifdef TEC_64
#include "loh/tree_be64.loh"
#else
#include "loh/tree_be32.loh"
#endif  
#else
#ifdef TEC_64
#ifdef WIN64
#include "loh/tree_le64w.loh"
#else
#include "loh/tree_le64.loh"
#endif  
#else
#include "loh/tree.loh"
#endif  
#endif  
#else
  iuplua_dofile("luatree.lua");
#endif

  return 1;
}
