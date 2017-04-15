require( "iuplua" )
require( "iupluacontrols" )

  mlist = iup.matrixlist{}
  mlist.count = 10
--  mlist["COUNT"] = 10
  mlist["VISIBLELINES"] = 5
  mlist["COLUMNORDER"] = "LABEL:COLOR:IMAGE"
--  mlist["COLUMNORDER"] = "LABEL:COLOR"
--  mlist["COLUMNORDER"] = "LABEL"
--  mlist["ACTIVE"] = "NO"
--  mlist["FOCUSCOLOR"] = "BGCOLOR"
  mlist["SHOWDELETE"] = "Yes"

  mlist["EDITABLE"] = "Yes"

  -- Bluish style
  if (1) then
    mlist["TITLE"] = "Test"
    mlist["BGCOLOR"] = "220 230 240"
    mlist["FRAMECOLOR"] = "120 140 160"
    mlist["ITEMBGCOLOR0"] = "120 140 160"
    mlist["ITEMFGCOLOR0"] = "255 255 255"
  end

  mlist["1"] = "AAA"
  mlist["2"] = "BBB"
  mlist["3"] = "CCC"
  mlist["4"] = "DDD"
  mlist["5"] = "EEE"
  mlist["6"] = "FFF"
  mlist["7"] = "GGG"
  mlist["8"] = "HHH"
  mlist["9"] = "III"
  mlist["10"] = "JJJ"

  mlist["COLOR1"] = "255 0 0"
  mlist["COLOR2"] = "255 255 0"
  --mlist["COLOR3"] = "0 255 0"
  mlist["COLOR4"] = "0 255 255"
  mlist["COLOR5"] = "0 0 255"
  mlist["COLOR6"] = "255 0 255"
  mlist["COLOR7"] = "255 128 0"
  mlist["COLOR8"] = "255 128 128"
  mlist["COLOR9"] = "0 255 128"
  mlist["COLOR10"] = "128 255 128"

  mlist["ITEMACTIVE3"] = "NO"
  mlist["ITEMACTIVE7"] = "NO"
  mlist["ITEMACTIVE8"] = "NO"

  mlist["IMAGEACTIVE9"] = "No"

  mlist["IMAGEVALUE1"] = "ON"
  mlist["IMAGEVALUE2"] = "ON"
  mlist["IMAGEVALUE3"] = "ON"

dlg = iup.dialog{iup.vbox{mlist; margin="10x10"}, title="IupMatrixList"}
dlg:showxy(iup.CENTER, iup.CENTER)

  mlist["APPENDITEM"] = "KKK"

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
