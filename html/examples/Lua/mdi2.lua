-- DATE : 2013-12-19 --

APPNAME = 'MDI Demo'

require('iuplua')

tree_nodes = 
{
  branchname = "Figures",
  "Other",
  {
    branchname = "triangle",
    state = "COLLAPSED",
    "equilateral",
    "isoceles",
    "scalenus",
  },
  {
    branchname = "parallelogram",
    "square",
    { leafname = "diamond", color = "92 92 255", titlefont = "Courier, 14" },
  },
  { branchname = "2D" },
  { branchname = "3D" },
}

tree = iup.tree{ RASTERSIZE = '200x'}

MDIMenu = iup.menu{}

MainForm = iup.dialog{
  menu = MDIMenu,
  TITLE = APPNAME,
  SIZE = 'HALFxHALF',
  MDIFRAME = 'YES',
  iup.vbox {
    iup.hbox {
	  iup.frame {
	    iup.hbox { iup.fill{}, iup.label { TITLE = 'IUP MDI TEST', FONT = 'Helvetica, 24', ALIGNMENT = 'ARIGHT:ACENTER' }, NULL }
	  }, NULL,
	},
	iup.hbox {
	  iup.split {
	    VALUE = '200',
  	    iup.vbox {
  	      iup.frame { tree, NULL }
	    },
	    iup.vbox {
 	      iup.frame {
	        iup.hbox { iup.fill{}, iup.label { TITLE = 'iup mdi test', ALIGNMENT = 'ARIGHT:ACENTER' }, NULL }
	      },
          iup.canvas { 
          EXPAND = 'YES', 
          MDICLIENT = 'YES', 
     --     BGCOLOR = '128 128 128' 
     			MDIMENU = MDIMenu,
          }, NULL
	    }, NULL,
	  },
	}, NULL, 
  },
}
MainForm:map()
iup.TreeAddNodes(tree, tree_nodes)
MainForm:show()

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
MDI1Form:show()

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end