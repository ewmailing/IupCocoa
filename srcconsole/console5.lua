require"iuplua"

-- Utilities
iup_console = {}

function iup_console.concat(str, info)
  return str .. info .. "\n"
end

function iup_console.print_version_info()
  iup_console.clear()
  local str = ""
  
  str = iup_console.concat(str, _COPYRIGHT) -- Only available in IupLuaConsole
  
  if (im) then str = iup_console.concat(str, "IM " .. im._VERSION .. "  " .. im._COPYRIGHT) end

  if (cd) then str = iup_console.concat(str, "CD " .. cd._VERSION .. "  " .. cd._COPYRIGHT) end

  str = iup_console.concat(str, "IUP " .. iup._VERSION .. "  " .. iup._COPYRIGHT)
  str = iup_console.concat(str, "")
  str = iup_console.concat(str, "IUP Info")
  str = iup_console.concat(str, "  System: " .. iup.GetGlobal("SYSTEM"))
  str = iup_console.concat(str, "  System Version: " .. iup.GetGlobal("SYSTEMVERSION"))

  local mot = iup.GetGlobal("MOTIFVERSION")
  if (mot) then str = iup_console.concat(str, "  Motif Version: " .. mot) end
  
  local gtk = iup.GetGlobal("GTKVERSION")
  if (gtk) then str = iup_console.concat(str, "  GTK Version: " .. gtk) end

  str = iup_console.concat(str, "  Screen Size: " .. iup.GetGlobal("SCREENSIZE"))
  str = iup_console.concat(str, "  Screen Depth: " .. iup.GetGlobal("SCREENDEPTH"))

  if (iup.GL_VENDOR) then str = iup_console.concat(str, "  OpenGL Vendor: " .. iup.GL_VENDOR) end
  if (iup.GL_RENDERER) then str = iup_console.concat(str, "  OpenGL Renderer: " .. iup.GL_RENDERER) end
  if (iup.GL_VERSION) then str = iup_console.concat(str, "  OpenGL Version: " .. iup.GL_VERSION) end
  
  print(str)
end

-- Console Dialog

iup_console.current_filename = nil -- Last file open
iup_console.mlCode = iup.multiline{expand="YES", visiblecolumns=60, visiblelines=20, font="Courier, 11"}
iup_console.mlConsole = iup.multiline{expand="YES", visiblecolumns=60, visiblelines=10, font="Courier, 11", readonly="Yes", bgcolor = "248 248 248"}
iup_console.txtCommand = iup.text{expand="HORIZONTAL", visiblecolumns=60, font="Courier, 11" }
iup_console.lblPosition = iup.label{title="0:0", expand="HORIZONTAL", alignment = "ARIGHT"}
iup_console.lblFileName = iup.label{title="", expand="HORIZONTAL"}

print_old = print
function print(...)
  local param = {...}
  local str = ""
  for i, k in ipairs(param) do 
    if (i > 1) then str = str .."\t" end
    str = str .. tostring(k)
  end
  iup_console.mlConsole.append = str
  iup_console.mlConsole.scrollto = "99999999:1"
end

function iup_console.value_to_string(v, is_table_index)
  if (type(v) == "string") then
    if (is_table_index) then 
      return "[\"" .. v .. "\"]"
    else
      return "\"" .. v .. "\""
    end
  else 
    return tostring(v)
  end
end

function iup_console.printtable(t)
  if (type(t) ~= "table") then 
    print("Error: Not a table") 
    print(t) 
  end
  local str = "{\n"
  for i, k in ipairs(t) do 
    str = str .. "  "..iup_console.value_to_string(k)..",\n"
  end
  for i, k in pairs(t) do 
    str = str .. "  "..tostring(i).. " = "..iup_console.value_to_string(k)..",\n"
  end
  str = str .. "}"
  print(str)
end

function iup_console.printvalue(v)
  if (type(v) == "table") then 
    iup_console.printtable(v)
  else
    print(iup_console.value_to_string(v))
  end
end

if (not loadstring) then
  loadstring = load
end

iup_console.cmd_history = {
}

function iup_console.txtCommand:k_any(k) 
  if (k == iup.K_CR) then 
    print("> " .. iup_console.txtCommand.value)
    table.insert(iup_console.cmd_history, iup_console.txtCommand.value)
    iup_console.cmd_history.current = #iup_console.cmd_history
    local cmd, msg = loadstring(iup_console.txtCommand.value)
    if (not cmd) then
      cmd = loadstring("return " .. iup_console.txtCommand.value)
    end
    if (not cmd) then
      print("Error: ".. msg) -- the original error message
    else
      local ret = cmd()
      if (ret) then
        iup_console.printvalue(ret)
      end
    end
    iup_console.txtCommand.value = ""
  elseif (k == iup.K_UP) then
    if (iup_console.cmd_history.current) then
      if (iup_console.cmd_history.current > 0) then
        iup_console.txtCommand.value = iup_console.cmd_history[iup_console.cmd_history.current]
        iup_console.cmd_history.current = iup_console.cmd_history.current - 1
        return iup.IGNORE
      end  
    end  
  elseif (k == iup.K_DOWN) then
    if (iup_console.cmd_history.current) then
      if (iup_console.cmd_history.current == 0) then iup_console.cmd_history.current = 1 end -- avoid repeating the first item
      if (iup_console.cmd_history.current < #iup_console.cmd_history) then
        iup_console.cmd_history.current = iup_console.cmd_history.current + 1
        iup_console.txtCommand.value = iup_console.cmd_history[iup_console.cmd_history.current]
        return iup.IGNORE
      end  
    end  
  end 
end

function iup_console.mlCode:caret_cb(lin, col)
  iup_console.lblPosition.title = lin..":"..col
end

function iup_console.clear()
  iup_console.mlCode.value=''  
  iup_console.lblFileName.title = ''  
  iup_console.current_filename = nil
end

iup_console.butExecute = iup.button{size="50x15", title="Execute (F5)", action="iup.dostring(iup_console.mlCode.value)"}
iup_console.butClearCommands = iup.button{size="50x15", title="Clear", action=iup_console.clear}
iup_console.butLoadFile = iup.button{size="50x15", title="Load..."}
iup_console.butSaveasFile = iup.button{size="50x15", title="Save As..."}
iup_console.butSaveFile = iup.button{size="50x15", title="Save (Ctrl+S)"}
iup_console.butExit = iup.button{size="50x15", title="Exit (Esc)", action="iup.ExitLoop()"}

function iup_console.SaveFile(filename)
  local newfile = io.open(filename, "w+")
  if (newfile) then
    newfile:write(iup_console.mlCode.value)
    newfile:close(newfile)
    print("FileSave:\n  "..filename)

    iup_console.current_filename = filename
    iup_console.lblFileName.title = iup_console.current_filename
  else
    error("Cannot Save file:\n  "..filename)
  end
end

function iup_console.butSaveFile:action()
  if (iup_console.current_filename == nil) then
    iup_console.butSaveasFile:action()
  else
    iup_console.SaveFile(iup_console.current_filename)
  end
end

function iup_console.butSaveasFile:action()
  local fd = iup.filedlg{
    dialogtype="SAVE", 
    nochangedir="NO", 
    directory=iup_console.last_directory,
    extfilter="Lua files|*.lua|All Files|*.*|",
    allownew="yes",
  }
                         
  fd:popup(iup.CENTERPARENT, iup.CENTERPARENT)
  local status = fd.status
  local filename = fd.value
  iup_console.last_directory = fd.directory
  fd:destroy()
  
  if status ~= "-1" then
    iup_console.SaveFile(filename)
  end
end

function iup_console.LoadFile(filename)
  local newfile = io.open (filename, "r")
  if (newfile == nil) then
    error ("Cannot load file:\n  "..filename)
  else
    iup_console.mlCode.value = newfile:read("*a")
    newfile:close (newfile)
    
    if IndentationLib then
      IndentationLib.textboxRecolor(iup_console.mlCode)
    end
    
    iup_console.current_filename = filename
    iup_console.lblFileName.title = iup_console.current_filename
  end
end

function iup_console.butLoadFile:action()
  local fd=iup.filedlg{
    dialogtype="OPEN", 
    nochangedir="NO", 
    parentdialog = iup_console.dlgMain,
    directory=iup_console.last_directory,
    extfilter="Lua files|*.lua|All Files|*.*|",
    allownew="NO"
  }
  fd:popup(iup.CENTERPARENT, iup.CENTERPARENT)
  local status = fd.status
  local filename = fd.value
  iup_console.last_directory = fd.directory
  fd:destroy()
  
  if (status == "0") then
    print("FileOpen:\n  "..filename)
    iup_console.LoadFile(filename)
  end
end

iup_console.vbxConsole = iup.hbox {
  iup.vbox{
    iup.label {title = "Filename: "},
    iup_console.butExecute,
    iup_console.butClearCommands,
    iup_console.butLoadFile,
    iup_console.butSaveFile,
    iup_console.butSaveasFile,
    iup_console.butExit,
    margin="0x0", 
  },
  iup.vbox{
    iup_console.lblFileName,
    iup_console.mlCode,
    iup.hbox { iup.label{title="Command Line:"}, iup_console.lblPosition, margin = "0x0", gap = 0},
    iup_console.txtCommand,
    iup_console.mlConsole,
    alignment = "ARIGHT",
    margin="0x0", 
  },
  alignment="ATOP", 
  margin="5x5", 
  gap="10",
}


-- Main Dialog Definition.

iup_console.dlgMain = iup.dialog{
  iup_console.vbxConsole,
  title="IupLua Console",
  dragdrop = "YES",
  defaultesc=iup_console.butExit,
  startfocus=iup_console.mlCode,
  close_cb = "iup.ExitLoop()",
  k_any = function(self, k) 
    if (k == iup.K_F5) then 
      iup.dostring(iup_console.mlCode.value) 
    elseif (k == iup.K_cS) then 
      iup_console.butSaveFile:action()
    end 
  end,
}

function iup_console.dlgMain:dropfiles_cb(filename, index, x, y)
  if (index == 0) then
    print("FileDropped:\n  "..filename)
    iup_console.LoadFile(filename)
  end
end

function iup_console.mlCode:dropfiles_cb(filename, index, x, y)
  if (index == 0) then
    print("FileDropped:\n  "..filename)
    iup_console.LoadFile(filename)
  end
end

if IndentationLib then
  IndentationLib.enable(iup_console.mlCode)
end

-- Displays the Main Dialog
iup_console.dlgMain:show()

iup_console.print_version_info()

iup.SetGlobal("GLOBALLAYOUTDLGKEY", "Yes")

if (arg[1]) then
  local filename = arg[1]
  print("FileCommandLine:\n  "..filename)
  iup_console.LoadFile(filename)
end

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()

  iup_console.dlgMain:destroy()

  if (im) then im.Close() end
  if (cd) then cd.Close() end
  iup.Close()
end
