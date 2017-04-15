require("iuplua")

local text1= iup.text{value="Apenas PARTE DESTE TEXTO deve ficar em highlight", size="250",
						readonly="YES", FORMATTING = "YES"}
local text2= iup.text{value="Nada deste texto deve ficar em highlight", size="250"}
local vbox= iup.vbox{text1,text2}

local tags = iup.user { selectionpos = "7:24", bgcolor = "255 128 64" }
text1.addformattag = tags

local dlg=iup.dialog{vbox}
dlg:show()

iup.MainLoop()
