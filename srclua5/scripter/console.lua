
local console = {}

if (not loadstring) then
  loadstring = load
end

function iup.ConsoleInit(txt_cmdline, mtl_output)
  console.cmdList = {}
  console.currentListInd = 0

  console.txtCmdLine = txt_cmdline 
  console.mtlOutput = mtl_output   
  
  console.hold_caret = false

  console.mtlOutput.value = _COPYRIGHT .. "\n" ..
                            "IUP " .. iup._VERSION .. "  " .. iup._COPYRIGHT
end


--------------------- Command History ---------------------


function iup.ConsoleKeyUpCommand()
  if #console.cmdList > 0 then
    if console.currentListInd >= 1 then
      console.txtCmdLine.value = console.cmdList[console.currentListInd]
      if console.currentListInd > 1 then
        console.currentListInd = console.currentListInd - 1
      end
    end
  end
end

function iup.ConsoleKeyDownCommand()
  if #console.cmdList > 0 then
    if console.currentListInd <= #console.cmdList then
      console.txtCmdLine.value = console.cmdList[console.currentListInd]
      if console.currentListInd < #console.cmdList then
        console.currentListInd = console.currentListInd + 1
      end
    end
  end
end

function iup.ConsoleEnterCommandStr(text)
  table.insert(console.cmdList, text)
  console.currentListInd = #console.cmdList
  iup.ConsolePrint("> " .. text)
end

function iup.ConsoleEnterCommand()

  local command = console.txtCmdLine.value
  if command == nil or command == "" then
    return
  end

  iup.ConsoleEnterCommandStr(command)

  local cmd, msg = loadstring(command)
  if (not cmd) then
    cmd = loadstring("return " .. command)
  end
  if (not cmd) then
    iup.ConsolePrint("Error: ".. msg) -- the original error message
  else
    local result = {pcall(cmd)}
    if result[1] then
      for i = 2, #result do
        iup.ConsolePrintValue(result[i])
      end
    else
      iup.ConsolePrint("Error: ".. result[2])
    end
  end

  console.txtCmdLine.value = ""
end


--------------------- Print Replacement ---------------------

function iup.ConsolePrint(...)
  local param = {...}
  local str = ""
  if (#param == 0) then
    str = "nil"
  else
    for i, k in ipairs(param) do 
      if (i > 1) then str = str .."\t" end
      str = str .. tostring(k)
    end
  end
  console.mtlOutput.append = str
  if (not console.hold_caret) then
    console.mtlOutput.scrollto = "99999999:1"
  end
end

print_old = print
print = iup.ConsolePrint

write_old = io.write
io.write = function(...)
  console.mtlOutput.appendnewline="No"
  iup.ConsolePrint(...)
  console.mtlOutput.appendnewline="Yes"
end

function iup.ConsoleValueToString(v)
  if (type(v) == "string") then
    return "\"" .. v .. "\""
  else 
    return tostring(v)
  end
end

function iup.ConsolePrintTable(t)
  local str = "{\n"
  local tmp = {}
  for i, k in ipairs(t) do 
    str = str .. "  ["..tostring(i).. "] = " .. iup.ConsoleValueToString(k) .. ",\n"
    tmp[i] = true
  end
  for i, k in pairs(t) do 
    if (not tmp[i]) then
      str = str .. "  ["..tostring(i).. "] = ".. iup.ConsoleValueToString(k) .. ",\n"
    end
  end
  str = str .. "}"

  iup.ConsolePrint(str)
end

function iup.ConsolePrintFunction(f)
  local info = debug.getinfo(f, "S")

  local defined
  if info.what == "C" then    
    defined = "  [Defined in C"
  else
    local s = string.sub(info.source, 1, 1)
    if s == "@" then
      local filename = string.sub(info.source, 2)
      defined = "  [Defined in the file: \"" .. filename .. "\""
    else
      local short_src = string.sub(info.short_src, 2, -2)
      defined = "  [Defined in a " .. short_src
    end
  end
  if info.linedefined > 0 then
      defined = defined .. ", line " .. info.linedefined .. "]"
  else
      defined = defined .. "]"
  end

  iup.ConsolePrint(defined)
end

function iup.ConsolePrintValue(v)
  if (type(v) == "table") then 
    iup.ConsolePrintTable(v)
  elseif (type(v) == "function") then 
    iup.ConsolePrintFunction(v)
  else
    iup.ConsolePrint(iup.ConsoleValueToString(v))
  end
end


--------------------- Utilities ---------------------


function iup.ConsoleListFuncs()

  iup.ConsoleEnterCommandStr("iup.ConsoleListFuncs()")

  console.hold_caret = true

  local global = _G
  local n,v = next(global, nil)
  while n ~= nil do
    if type(v) == "function" then
      iup.ConsolePrint(n)
    end
    n,v = next(global, n)
  end

  console.hold_caret = false
  console.mtlOutput.scrollto = "99999999:1"
end

function iup.ConsoleListVars()

  iup.ConsoleEnterCommandStr("iup.ConsoleListVars()")

  console.hold_caret = true

  local global = _G
  local n,v = next(global, nil)
  while n ~= nil do
    if type(v) ~= "function" and n ~= "_G" then
      iup.ConsolePrint(n)
    end
    n,v = next(global, n)
  end

  console.hold_caret = false
  console.mtlOutput.scrollto = "99999999:1"
end

function iup.ConsolePrintStack()
  local info, desc, defined 
  
  iup.ConsoleEnterCommandStr("iup.ConsolePrintStack()")

  local level = 2 -- skip level 0, getinfo
                  -- skip level 1, ConsolePrintStack

  info = debug.getinfo(level, "Snl") -- source, name, namewhat, what, currentline, linedefined
  while  info ~= nil do
    if info.what == "main" then
      desc = "<main>"
    elseif info.name and info.name ~= "" then
      desc = info.name
    else
      desc = "<noname>"
    end
    if info.namewhat ~= "" then
        desc = desc .. " <".. info.namewhat .. ">"
    end
    if info.currentline > 0 then
       desc = desc .. " at line " .. info.currentline
    end

    if info.what == "C" then    
      defined = "  [Defined in C"
    else
      local s = string.sub(info.source, 1, 1)
      if s == "@" then
        local filename = string.sub(info.source, 2)
        defined = "  [Defined in the file: \"" .. filename .. "\""
      else
        local short_src = string.sub(info.short_src, 2, -2)
        defined = "  [Defined in a " .. short_src
      end
    end
    if info.linedefined > 0 then
       defined = defined .. ", line " .. info.linedefined .. "]"
    else
       defined = defined .. "]"
    end

    iup.ConsolePrint(level - 1 .. " - " .. desc .. defined)

    level = level + 1
    info = debug.getinfo(level, "Snl") -- source, name, namewhat, what, currentline, linedefined
  end
 
end

