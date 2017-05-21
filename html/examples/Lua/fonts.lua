require( "iuplua" )

l = iup.list{dropdown="YES"} 

iup.SetAttribute(l, "1", "Helvetica, 8") 
iup.SetAttribute(l, "2", "Courier, 8") 
iup.SetAttribute(l, "3", "Times, 8") 
iup.SetAttribute(l, "4", "Helvetica, Italic 8") 
iup.SetAttribute(l, "5", "Courier, Italic 8") 
iup.SetAttribute(l, "6", "Times, Italic 8") 
iup.SetAttribute(l, "7", "Helvetica, Bold 8") 
iup.SetAttribute(l, "8", "Courier, Bold 8") 
iup.SetAttribute(l, "9", "Times, Bold 8") 
iup.SetAttribute(l, "10", "Helvetica, 10") 
iup.SetAttribute(l, "11", "Courier, 10") 
iup.SetAttribute(l, "12", "Times, 10") 
iup.SetAttribute(l, "13", "Helvetica, Italic 10") 
iup.SetAttribute(l, "14", "Courier, Italic 10") 
iup.SetAttribute(l, "15", "Times, Italic 10") 
iup.SetAttribute(l, "16", "Helvetica, Bold 10") 
iup.SetAttribute(l, "17", "Courier, Bold 10") 
iup.SetAttribute(l, "18", "Times, Bold 10") 
iup.SetAttribute(l, "19", "Helvetica, 12") 
iup.SetAttribute(l, "20", "Courier, 12") 
iup.SetAttribute(l, "21", "Times, 12") 
iup.SetAttribute(l, "22", "Helvetica, Italic 12") 
iup.SetAttribute(l, "23", "Courier, Italic 12") 
iup.SetAttribute(l, "24", "Times, Italic 12") 
iup.SetAttribute(l, "25", "Helvetica, Bold 12") 
iup.SetAttribute(l, "26", "Courier, Bold 12") 
iup.SetAttribute(l, "27", "Times, Bold 12") 
iup.SetAttribute(l, "28", "Helvetica, 14") 
iup.SetAttribute(l, "29", "Courier, 14") 
iup.SetAttribute(l, "30", "Times, 14") 
iup.SetAttribute(l, "31", "Helvetica, Italic 14") 
iup.SetAttribute(l, "32", "Courier, Italic 14") 
iup.SetAttribute(l, "33", "Times, Italic 14") 
iup.SetAttribute(l, "34", "Helvetica, Bold 14") 
iup.SetAttribute(l, "35", "Courier, Bold 14") 
iup.SetAttribute(l, "36", "Times, Bold 14") 

dg = iup.dialog{l} 
dg.title = "title" 

dg2 = nil 

l.action = function(self, t, i ,v) 

  if dg2 then 
    iup.Hide(dg2) 
  end 

  if v == 1 then 
    ml = iup.multiline{} 
    ml.size = "200x200" 
    ml.value = "1234\nmmmmm\niiiii" 

    ml.font = t 

    dg2 = iup.dialog{ml} 
    dg2.title = t 
    dg2:show() 
    iup.SetFocus(l) 
  end 
end 

dg:show() 

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
