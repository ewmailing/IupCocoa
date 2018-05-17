-- Recommend running bin2c.lua on this to use in lua_app_main.c
-- lua ../../../srclua5/bin2c.lua testscript.lua  > testscript.h
-- Depending if modules instead of dynamic libraries, 
-- you will need to change the call to iuplua_dobuffer in the generated .h to my_iuplua_dobuffer.
-- The function iuplua_dobuffer is inside the iuplua module, which is not accessible since the module isn't loaded yet (chicken and egg problem).
-- I have a quick and dirty replacement called my_iuplua_dobuffer in the C code to workaround it.


-- Problem: If iuplua* are built as modules, on some platforms such as Darwin, we cannot use require.
-- The problem is that iupluaweb depends on symbols in iuplua (_iup_call).
-- We are not allowed to link modules against other modules on Darwin.
-- And when we try to require iupluaweb, it will fail because it can't find the symbol.
-- Lua 5.2 seems to have recognized this cross-dependency problem in general, and offers package.loadlib
-- which allows you to force dlopen using RTLD_GLOBAL instead of RTLD_LOCAL which will make the symbol visible.
-- But this creates other downsides including:
-- 1. It must be called before anybody else tries to call require() or the cache will avoid loading it again.
-- 2. An explicit path must be provided.
-- 3. It is not really guaranteed to be portable.
-- http://lua-users.org/lists/lua-l/2015-05/msg00263.html	

--[[
-- Warning: You must set the package.cpath and package.path to things that make sense for your platform. 
-- Lua and IUP currently have no mechanisms to help you do the real-world reasonable thing,
-- e.g. on Mac, you would look in the app bundle's PlugIns subdirectory for modules and somewhere in Resources for Lua scripts.
print("package.cpath: ", package.cpath)
package.cpath="/Users/ewing/Source/GIT/IupMac/iup315/BUILD/OSXModule/Debug/?.so;"..package.cpath;
print("package.cpath: ", package.cpath)


-- Ensure the library is loaded as RTLD_GLOBAL because iupluaweb uses symbols from iuplua which are not accessible on Mac through RTLD_LOCAL (default of require()).
-- Once you have the package.cpath set up, we can use package.searchpath to help us pass the right thing to package.loadlib.
--package.loadlib("/Users/ewing/Source/GIT/IupMac/iup315/BUILD/OSXModule/Debug/iuplua.so", "*")
package.loadlib(package.searchpath("iuplua", package.cpath), "*")
--require("iuplua")
require("iupluaweb")
--]]

-- If we build as dynamic libraries and load from C, then we must skip the above. This has a desirable property of allowing us to link directly against things and 

function IupExitPoint()
	iup.Log("DEBUG", "Quitting");
	iup.Close()
end
function IupEntryPoint()
	iup.Log("DEBUG", "IupEntryPoint start");
	local the_label = iup.label({title="Hello World!"});
	if(the_label) then
		iup.Log("DEBUG", "the_label is defined");
	else
		iup.Log("DEBUG", "the_label is nil");
	end
	local web_view = iup.webbrowser({value="https://www.google.com"})
	if(web_view) then
		iup.Log("DEBUG", "web_view is defined");
	else
		iup.Log("DEBUG", "web_view is nil");
	end

	local v_box = iup.vbox({the_label, web_view})
	local the_dialog = iup.dialog(
		{
			title = "Iup Lua",
			size = "HALFxHALF",
			v_box
		}

	);
	the_dialog:show();
end


