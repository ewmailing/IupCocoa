require("iuplua")
require( "iupluacontrols" )
require("iupluaimglib")

label_1 = iup.label {title = "The label                "}
label_2 = iup.label {image = "IUP_ActionOk"}

label_3 = iup.animatedlabel{}
label_3.animation = "IUP_CircleProgressAnimation"
label_3.start = "Yes"

vbox = iup.vbox{
                 label_1
                ,label_2
                , label_3
}


dlg = iup.dialog{
  vbox
}


dlg:showxy(iup.CENTER,iup.CENTER)
dlg.usersize = nil

-- to be able to run this script inside another context
if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
  iup.Close()
end