-- Recommend running bin2c.lua on this to use in lua_app_main.c
-- lua ../../../srclua5/bin2c.lua testscript.lua  > testscript.h


-- require("iuplua") -- is already done for you from C. (Note: iupluaweb and others will cause problems...that is why they are done in C.)

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


