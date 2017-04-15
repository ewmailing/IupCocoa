
APPNAME = 'MDI Demo'

require('iuplua')

MDIMenu = iup.menu{}

MainForm = iup.dialog{
    menu = MDIMenu,
	TITLE = APPNAME,
	SIZE = 'HALFxHALF',
	MDIFRAME = 'YES',
		iup.canvas{
			EXPAND = 'YES',
			MDICLIENT = 'YES',
			MDIMENU = MDIMenu,
		},
}

MDI1Form = iup.dialog{
	TITLE = 'MDI1',
	SIZE = '200x150',
	MDICHILD = 'YES',
	PARENTDIALOG = MainForm,
	iup.text{
		EXPAND = 'YES',
		MULTILINE = 'YES',
	},
}

MDI2Form = iup.dialog{
	TITLE = 'MDI1',
	SIZE = '200x150',
	MDICHILD = 'YES',
	PARENTDIALOG = MainForm,
	iup.text{
		EXPAND = 'YES',
		MULTILINE = 'YES',
	},
}

MainForm:show()
MDI1Form:show()
MDI2Form:show()

if (iup.MainLoopLevel()==0) then
   iup.MainLoop()
end
