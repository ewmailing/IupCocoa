-- IupToggle Example in IupLua 
require( "iuplua" )

img1 = iup.image{
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,2,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,2,2,2,2,2,2,2,2,2,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
       colors = {"255 255 255", "0 192 0"}
}

img2 = iup.image{
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,2,2,2,2,2,2,1,1,1,1,1,1},
       {1,1,1,2,1,1,1,1,1,1,2,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,2,2,2,2,2,2,2,2,2,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
       colors = {"255 255 255", "0 192 0"}
}

toggle1 = iup.toggle{title = "", image = img1}
toggle2 = iup.toggle{title = "inactive toggle with image", image = img2, active="NO"}
toggle3 = iup.toggle{title = "regular toggle"}
toggle4 = iup.toggle{title = "toggle with blue foreground color", fgcolor = "0 0 255" }
toggle5 = iup.toggle{title = "inactive toggle", active = "NO" }
toggle6 = iup.toggle{title = "toggle with Courier 14 Bold font", font = "Courier, Bold 14" }
toggle7 = iup.toggle{title = "toggle with size 30x10", size = "80x20" }

function toggle1:action(v)
   if v == 1 then state = "pressed" else state = "released" end
   iup.Message("Toggle 1",state)
end

box = iup.vbox{ 
                 toggle1,
                 toggle2,
                 toggle3,
                 toggle4,
                 toggle5,
                 toggle6,
                 toggle7,
               }
                
dlg = iup.dialog{box; title = "IupToggle", margin="5x5", gap="5", resize="NO"}
dlg:showxy(iup.CENTER, iup.CENTER)

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
