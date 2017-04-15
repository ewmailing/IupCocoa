-- StartHTML = byte count to just before <HTML> in this case, could be a more complex header
-- EndHTML = byte count to the last byte, not counting the extra NUL
-- StartFragment = byte count just after the <!--StartFragment-->
-- EndFragment = byte count just before the <!--EndFragment-->

-- Notice that byte count must include the line feed, 
--   if DOS format is used must count LF by 2
-- Notice thatcharacter encoding is UTF-8, 
--   so if string has extended characters must converto to UTF-8 before copying

-- Tested on Windows

html = [[Version:0.9
StartHTML:000084
EndHTML:000242
StartFragment:000163
EndFragment:000207
<HTML>
<head>
<title>HTML clipboard</title>
</head>
<body>
<!--StartFragment-->a <strong>copy</strong> <em>to</em> the clip<!--EndFragment-->
</body>
</html>
]] .. "\0"  -- Extra NUL for UTF-8

clipboard = iup:clipboard()

clipboard.text = nil -- clear contents

clipboard.text = "a copy to the clip" -- copy also as text

clipboard.addformat = 'HTML Format' -- copy as HTML
clipboard.format = 'HTML Format'
clipboard.formatdatasize = html:len()+1
clipboard.formatdata = html
