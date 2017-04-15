require('iuplua')
require('iupluaweb')

local MyWeb = iup.webbrowser{}

MyWeb.HTML = [[
<HTML>
<BODY>
<A HREF="<http://www.lua.org>">Hello Lua</A> 
</BODY> 
</HTML> ]]

first = 1
function MyWeb:navigate_cb(url)
  print('url=', url)
  if (first == 1) then
    first = 0
  else
    return iup.IGNORE
  end
end

local MainForm = iup.dialog{
  title = 'HELLO',
  size = 'HALFxHALF',
  MyWeb,
}

MainForm:show()
