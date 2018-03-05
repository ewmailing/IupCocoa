local DEBUG_INACTIVE = 1 -- debug is inactive, hooks are not set
local DEBUG_ACTIVE = 2 -- debug should be active and running, until we found a breakpoint or the program ends
local DEBUG_STOPPED = 3 -- debug should be active and running, we are waiting to stop in the next opportunity, we have to abort
local DEBUG_STEP_INTO = 4 -- debug should be active and running, we are waiting until its steps into the function to pause, or we found a breakpoint
local DEBUG_STEP_OVER = 5 -- debug should be active and running, we are waiting until its steps over the function to pause, or we found a breakpoint
local DEBUG_STEP_OUT = 6 -- debug should be active and running, we are waiting until its steps out of the function to pause, or we found a breakpoint
local DEBUG_PAUSED = 7 -- debug should be active, but paused
local DEBUG_ERRORPAUSED = 8 -- debug should be active, but paused in an error

local FUNC_STATE_INSIDE = 1
local FUNC_STATE_OUTSIDE = 2

local debugger = {
  debugState = DEBUG_INACTIVE,

  currentFuncLevel = 0,
  stepFuncLevel = 0,
  stepFuncState = 0,

  startFile = nil,
  currentLine = nil,
  currentFile = nil,

  breakpoints = {},

  main_dialog = nil,
}

function iup.DebuggerInit(main_dialog)
  debugger.main_dialog = main_dialog
end

function iup.DebuggerGetCurrentMultitext()
  local tabs = iup.GetDialogChild(debugger.main_dialog, "TABS")
  return tabs.value_handle
end

function iup.DebuggerFindMultitext(filename)
  local tabs = iup.GetDialogChild(debugger.main_dialog, "TABS")

  local multitext = iup.GetChild(tabs, 0)
  while multitext do
    if multitext.filename == filename then
      return multitext
    end

    multitext = iup.GetBrother(multitext)
  end
end

function iup.DebuggerRestoreBreakpoints(multitext, m_filename)
  local list_break = iup.GetDialogChild(debugger.main_dialog, "LIST_BREAK")

  local index = 1
  local filename = list_break["FILENAME" .. index]
  while filename do
    if filename == m_filename then
      local line = tonumber(list_break["LINE" .. index])
      multitext["MARKERADD".. (line - 1)] = 1 -- (margin=1)
    end

    index = index + 1
    filename = list_break["FILENAME" .. index]
  end
end

function iup.DebuggerOpenMultitext(filename, source)
  local tabs = iup.GetDialogChild(debugger.main_dialog, "TABS")
  local old_count = tonumber(tabs.count)

  local s = string.sub(filename, 1, 6)
  if s == "string" then
    debugger.main_dialog.newfile = nil
  else
    debugger.main_dialog.openfile = filename
  end

  local count = tonumber(tabs.count)
  if count > old_count then
    local multitext = iup.GetChild(tabs, old_count)

    if s == "string" then
      multitext.filename = filename
      multitext.value = source
      multitext.temporary = "Yes"

      iup.DebuggerRestoreBreakpoints(multitext, filename)
    end

    if iup.DebuggerIsActive() then
      multitext.readonly = "Yes"
    end

    return multitext
  end
end

function iup.DebuggerCloseTemporary()
  local tabs = iup.GetDialogChild(debugger.main_dialog, "TABS")

  local multitext = iup.GetChild(tabs, 0)
  local pos = 0
  while multitext do
    if multitext.temporary == "Yes" then
      local temp = multitext
      multitext = iup.GetBrother(multitext)

      iup.SetAttribute(debugger.main_dialog, "FORCECLOSEFILE", pos)
    else
      multitext = iup.GetBrother(multitext)
      pos = pos + 1
    end
  end
end

function iup.DebuggerSetAttribAllMultitext(name, value)
  local tabs = iup.GetDialogChild(debugger.main_dialog, "TABS")

  local multitext = iup.GetChild(tabs, 0)
  while multitext do
    multitext[name] = value 
    multitext = iup.GetBrother(multitext)
  end
end

function iup.DebuggerSetDialogChildAttrib(child_name, name, value)
  iup.SetAttribute(iup.GetDialogChild(debugger.main_dialog, child_name), name, value)
end

local function setparent_param_cb(param_dialog, param_index)
  if param_index == iup.GETPARAM_MAP then
    param_dialog.parentdialog = debugger.main_dialog
  end

  return 1
end


------------------------------------- User Interface State -------------------------------------


function iup.DebuggerSetStateString(state)
  local map_state = {  -- only the ones used in C
    DEBUG_ACTIVE = DEBUG_ACTIVE,
    DEBUG_STOPPED = DEBUG_STOPPED,
    DEBUG_STEP_INTO = DEBUG_STEP_INTO,
    DEBUG_STEP_OVER = DEBUG_STEP_OVER,
    DEBUG_STEP_OUT = DEBUG_STEP_OUT,
    DEBUG_PAUSED = DEBUG_PAUSED,
  }

  iup.DebuggerSetState(map_state[state])
end

function iup.DebuggerIsActive()
  if debugger.debugState ~= DEBUG_INACTIVE then
    return true
  else
    return false
  end
end

function iup.DebuggerSetState(st)
  local stop, step, pause, run, dbg, curline

  if debugger.debugState == st then
    return
  end

  iup.DebuggerSetAttribAllMultitext("READONLY", "Yes")

  if st == DEBUG_STOPPED then

    stop = "NO"
    step = "NO"
    run = "NO"
    pause = "NO"
    dbg = "NO"
    curline = "NO"
  elseif st == DEBUG_ACTIVE or st == DEBUG_STEP_INTO or st == DEBUG_STEP_OVER or st == DEBUG_STEP_OUT then
    stop = "YES"
    step = "NO"
    run = "NO"
    pause = "YES"
    dbg = "NO"
    curline = "NO"

    if st == DEBUG_STEP_OUT then
      debugger.stepFuncLevel = debugger.currentFuncLevel
      debugger.stepFuncState = FUNC_STATE_INSIDE
    else
      debugger.stepFuncLevel = 0
      debugger.stepFuncState = FUNC_STATE_OUTSIDE
    end
  elseif st == DEBUG_PAUSED then
    stop = "YES"
    step = "YES"
    run = "NO"
    pause = "NO"
    dbg = "YES"
    curline = "Yes"
  elseif st == DEBUG_ERRORPAUSED then
    stop = "YES"
    step = "NO"
    run = "NO"
    pause = "NO"
    dbg = "NO"
    curline = "Yes"
  else -- st == DEBUG_INACTIVE
    stop = "NO"
    step = "NO"
    run = "YES"
    pause = "NO"
    dbg = "YES"
    curline = "NO"

    iup.DebuggerSetAttribAllMultitext("READONLY", "No")

    iup.DebuggerClearLocalVariablesTree()
    iup.DebuggerClearStackList()
    iup.DebuggerInitGlobalsTree()
  end
    
  debugger.debugState = st

  iup.DebuggerSetAttribAllMultitext("MARKERDELETEALL", "2") -- clear all line highlight (margin=2)
  iup.DebuggerSetAttribAllMultitext("MARKERDELETEALL", "3") -- clear all line arrow (margin=3)

  iup.DebuggerSetDialogChildAttrib("ITM_RUN", "ACTIVE", run)
  iup.DebuggerSetDialogChildAttrib("ITM_STOP", "ACTIVE", stop)
  iup.DebuggerSetDialogChildAttrib("ITM_PAUSE", "ACTIVE", pause)
  iup.DebuggerSetDialogChildAttrib("ITM_DEBUG", "ACTIVE", dbg)
  iup.DebuggerSetDialogChildAttrib("ITM_STEPINTO", "ACTIVE", step)
  iup.DebuggerSetDialogChildAttrib("ITM_STEPOVER", "ACTIVE", step)
  iup.DebuggerSetDialogChildAttrib("ITM_STEPOUT", "ACTIVE", step)
  iup.DebuggerSetDialogChildAttrib("ITM_CURRENTLINE", "ACTIVE", curline)
  iup.DebuggerSetDialogChildAttrib("BTN_RUN", "ACTIVE", run)
  iup.DebuggerSetDialogChildAttrib("BTN_STOP", "ACTIVE", stop)
  iup.DebuggerSetDialogChildAttrib("BTN_PAUSE", "ACTIVE", pause)
  iup.DebuggerSetDialogChildAttrib("BTN_DEBUG", "ACTIVE", dbg)
  iup.DebuggerSetDialogChildAttrib("BTN_STEPINTO", "ACTIVE", step)
  iup.DebuggerSetDialogChildAttrib("BTN_STEPOVER", "ACTIVE", step)
  iup.DebuggerSetDialogChildAttrib("BTN_STEPOUT", "ACTIVE", step)
  iup.DebuggerSetDialogChildAttrib("BTN_CURRENTLINE", "ACTIVE", curline)
end                   

function iup.DebuggerHighlightLine(filename, line, source)
  if line == nil or line <= 0 then
    return
  end

  debugger.currentLine = line
  debugger.currentFile = filename

  local multitext = iup.DebuggerGetMultitext(filename, true, true, source) -- find and open if necessary
  if not multitext then
    return
  end

  local pos = iup.TextConvertLinColToPos(multitext, line-1, 0) -- line here starts at 0
  multitext.caretpos = pos

  -- highlight
  multitext["MARKERADD"..(line-1)] = 2 -- (margin=2)
  -- arrow
  multitext["MARKERADD"..(line-1)] = 3 -- (margin=3)
end

function iup.DebuggerShowCurrentLine()
  if debugger.currentLine and debugger.currentFile then
    iup.DebuggerSelectLine(debugger.currentFile, debugger.currentLine, true) -- find
  end
end

function iup.DebuggerGetMultitext(filename, find, open, source)
  local multitext = iup.DebuggerGetCurrentMultitext()

  if filename ~= multitext.filename then
    if find then
      multitext = iup.DebuggerFindMultitext(filename)
      if open then
        local tabs = iup.GetDialogChild(debugger.main_dialog, "TABS")
        tabs.value = multitext
        debugger.main_dialog.updatetitle = "Yes"

        if not multitext then
          multitext = iup.DebuggerOpenMultitext(filename, source)
          if not multitext then
            return
          end
        end
      end
    else
      return
    end
  end

  return multitext
end

function iup.DebuggerSelectLine(filename, line, find, source)
  if line == nil or line <= 0 then
    return
  end

  local multitext = iup.DebuggerGetMultitext(filename, find, true, source) -- find and always open if necessary
  if not multitext then
    return
  end

  local pos = iup.TextConvertLinColToPos(multitext, line-1, 0) -- line here starts at 0
  multitext.caretpos = pos

  -- select
  multitext.selection = line-1 .. ",0:" .. line-1 .. ",9999"
end


------------------------------------- Breakpoints -------------------------------------

function iup.DebuggerHasBreakpoint(filename, line)
  local file_breaks = debugger.breakpoints[filename]
  if file_breaks and file_breaks[line] then
    return true
  end
  return false
end

function iup.DebuggerRestoreLastListValue(list, last_value)
  -- maintain visual position of the selection on the list, not necessarily the same item
  last_value = tonumber(last_value)
  local count = tonumber(list.count)
  if last_value > count then
    list.value = count
  else
    list.value = last_value
  end
end

function table.count(t)
  local count = 0
  for i, v in pairs(t) do
    count = count + 1
  end
  return count
end

function iup.DebuggerUpdateBreakpointsListLines(list_break, m_filename, m_line, new_line)
  local count = tonumber(list_break.count)
  for i = 1, count do
    local filename = list_break["FILENAME" .. i]
    local line = tonumber(list_break["LINE" .. i])

    if m_filename == filename and m_line == line then
      -- update list (just title, index remains the same)
      list_break[i] = "Line " .. new_line .. " of \"" .. filename .. "\""
        
      -- update FILENAME#LINE
      list_break["LINE" .. i] = new_line
    end
  end
end

function iup.DebuggerRemoveBreakpointsListItem(list_break, m_filename, m_line)
  local count = tonumber(list_break.count)
  for i = 1, count do
    local filename = list_break["FILENAME" .. i]
    local line = tonumber(list_break["LINE" .. i])

    if m_filename == filename and m_line == line then
      iup.DebuggerRemoveBreakpointsListIndex(list_break, i)
    end
  end
end

function iup.DebuggerRemoveBreakpointsListIndex(list_break, index)
  -- update list
  list_break.removeitem = index
  local count = tonumber(list_break.count)

  -- update FILENAME#LINE
  for i = index, count do
    list_break["FILENAME"..i] = list_break["FILENAME"..(i+1)]
    list_break["LINE"..i] = list_break["LINE"..(i+1)]
  end
  list_break["FILENAME" .. count+1] = nil
  list_break["LINE" .. count+1] = nil
end

function iup.DebuggerRemoveBreakpoint(list_break, index)
  local last_value = list_break.value
  local line = tonumber(list_break["LINE" .. index])
  local filename = list_break["FILENAME" .. index]
  local count = tonumber(list_break.count)
  
  -- update multitext
  local multitext = iup.DebuggerGetMultitext(filename, true, false) -- find but do NOT open if not found
  if multitext then
    multitext["MARKERDELETE" .. (line - 1)] = 1 -- margin=1
  end

  -- update breakpoints table
  local file_breaks = debugger.breakpoints[filename]
  if file_breaks then 
    file_breaks[line] = nil
    if table.count(file_breaks) == 0 then
      debugger.breakpoints[filename] = nil
    end
  end

  -- update list and FILENAME#LINE
  iup.DebuggerRemoveBreakpointsListIndex(list_break, index)

  if count == 1 then
    -- update buttons, it is now empty
    iup.DebuggerSetDialogChildAttrib("REMOVE_BREAK", "ACTIVE", "NO")
  else
    iup.DebuggerRestoreLastListValue(list_break, last_value)
  end
end

function iup.DebuggerAddBreakpoint(list_break, filename, line)
  local last_value = list_break.value

    -- update multitext
    local multitext = iup.DebuggerGetMultitext(filename, true, false) -- find but do NOT open if not found
    if multitext then
      multitext["MARKERADD" .. (line - 1)] = 1 -- margin=1
    end

  -- update breakpoints table
  local file_breaks = debugger.breakpoints[filename]
  if not file_breaks then 
    file_breaks = {} 
    debugger.breakpoints[filename] = file_breaks
  end
  file_breaks[line] = { } -- other breakpoint info can be saved here

  -- update list
  list_break.appenditem = "Line " .. line .. " of \"" .. filename .. "\""
  local count = tonumber(list_break.count)

  -- update FILENAME#LINE
  list_break["FILENAME"..count] = filename
  list_break["LINE"..count] = line

  if count == 1 then
    -- update buttons, it was empty
    iup.DebuggerSetDialogChildAttrib("REMOVE_BREAK", "ACTIVE", "Yes")
  end

  -- select the new item
  list_break.value = count
end

function table_compare_lines(elem1, elem2)
  if (elem1.line < elem2.line) then
    return true
  else
    return false
  end
end

function iup.DebuggerBreakpointsChanged(multitext, start_lin, num_lin)
  local filename = multitext.filename
  local list_break = iup.GetDialogChild(debugger.main_dialog, "LIST_BREAK")
  local last_value = list_break.value

  local file_breaks = debugger.breakpoints[filename]
  if not file_breaks then 
    return
  end

  local new_file_breaks = {}

  for line, bp in pairs(file_breaks) do
    if line >= start_lin then
      if num_lin < 0 and line < start_lin - num_lin then
        -- removed lines and removed breakpoint

        -- update multitext
        multitext["MARKERDELETE" .. (start_lin - 1)] = 1 -- margin=1    -- all breakpoints inside the region are collapsed to the start_lin

        -- update breakpoints table
        -- simply don't copy to new_file_breaks

        -- update list and FILENAME#LINE
        iup.DebuggerRemoveBreakpointsListItem(list_break, filename, line)
      else
        -- added or removed lines, just change line in breakpoints

        -- update breakpoints table
        -- copy breakpoint at the new position
        new_file_breaks[line + num_lin] = bp

        -- update list and FILENAME#LINE
        iup.DebuggerUpdateBreakpointsListLines(list_break, filename, line, line + num_lin)
      end
    else
      -- no changes, just copy
      new_file_breaks[line] = bp
    end
  end

  -- update breakpoints table
  if table.count(new_file_breaks) == 0 then
    debugger.breakpoints[filename] = nil
  else
    debugger.breakpoints[filename] = new_file_breaks
  end

  local count = tonumber(list_break.count)
  if count == 0 then
    -- update buttons, it is now empty
    iup.DebuggerSetDialogChildAttrib("REMOVE_BREAK", "ACTIVE", "NO")
  else
    iup.DebuggerRestoreLastListValue(list_break, last_value)
  end
end

function iup.DebuggerRemoveAllBreakpoints()
  local list_break = iup.GetDialogChild(debugger.main_dialog, "LIST_BREAK")

  -- update all multitext
  iup.DebuggerSetAttribAllMultitext("MARKERDELETEALL", "1") -- margin=1

  -- update list
  list_break.removeitem = "ALL"

  -- update breakpoints table
  debugger.breakpoints = {}
  
  -- update FILENAME#LINE
  local index = 1
  local filename = list_break["FILENAME" .. index]
  while filename do
    list_break["FILENAME" .. index] = nil
    list_break["LINE" .. index] = nil

    index = index + 1
    filename = list_break["FILENAME" .. index]
  end

  -- update buttons
  iup.DebuggerSetDialogChildAttrib("REMOVE_BREAK", "ACTIVE", "NO")
end

function iup.DebuggerInitBreakpointsList(list_break)
  -- From FILENAME#LINE, initialize list and breakpoints table
  -- each multitext will be updated when created/loaded, see restoremarkers_cb

  list_break.removeitem = "ALL"
  debugger.breakpoints = {}

  local index = 1
  local filename = list_break["FILENAME" .. index]
  while filename do
    local line = tonumber(list_break["LINE" .. index])

    -- update list
    list_break[index] = "Line " .. line .. " of \"" .. filename .. "\""

    -- update breakpoints table
    local file_breaks = debugger.breakpoints[filename]
    if not file_breaks then 
      file_breaks = {} 
      debugger.breakpoints[filename] = file_breaks
    end
    file_breaks[line] = { } -- other breakpoint info can be saved here

    index = index + 1
    filename = list_break["FILENAME" .. index]
  end

  -- update buttons
  if index > 1 then
    iup.DebuggerSetDialogChildAttrib("REMOVE_BREAK", "ACTIVE", "Yes")

    list_break.value = 1 -- select first item on list
  else
    iup.DebuggerSetDialogChildAttrib("REMOVE_BREAK", "ACTIVE", "NO")
  end
end

function iup.DebuggerAddBreakpointList()
  local multitext = iup.DebuggerGetCurrentMultitext()
  local suggest_filename = multitext.filename
  if (not suggest_filename) then suggest_filename = "" end
  local suggest_lin, suggest_col = iup.TextConvertPosToLinCol(multitext, multitext.caretpos)

  local status, filename, line = iup.GetParam("Add Breakpoint", setparent_param_cb, "Filename: %s\nLine: %i[1,]\n", suggest_filename, suggest_lin + 1)

  if (status) then
    local list_break = iup.GetDialogChild(debugger.main_dialog, "LIST_BREAK")

    if iup.DebuggerHasBreakpoint(filename, line) then
      iup.MessageError(debugger.main_dialog, "Breakpoint already exists.")
      return
    end

    iup.DebuggerAddBreakpoint(list_break, filename, line)
  end
end

function iup.DebuggerBreaksListAction(list_break, index)
  local filename = list_break["FILENAME" .. index]
  local line = tonumber(list_break["LINE" .. index])
  
  iup.DebuggerSelectLine(filename, line, false) -- do not find
end

function iup.DebuggerBreaksListActivate(list_break, index)
  local filename = list_break["FILENAME" .. index]
  local line = tonumber(list_break["LINE" .. index])

  iup.DebuggerSelectLine(filename, line, true) -- find
end


------------------------------------- Locals -------------------------------------


function iup.DebuggerClearLocalVariablesTree()
  iup.DebuggerSetDialogChildAttrib("PRINT_LOCAL", "ACTIVE", "NO")
  iup.DebuggerSetDialogChildAttrib("SET_LOCAL", "ACTIVE", "NO")

  iup.DebuggerSetDialogChildAttrib("TREE_LOCAL", "DELNODE", "ALL")
end

function iup.DebuggerSetLocalVariable()
  local tree_local = iup.GetDialogChild(debugger.main_dialog, "TREE_LOCAL")
  local id = tree_local.value
  if (not id or tonumber(id) < 0) then
    iup.MessageError(debugger.main_dialog, "Select a variable on the list.")
    return
  end
  
  local userdata = iup.TreeGetUserId(tree_local, id)

  local name = userdata.name
  local value = userdata.value
  
  if (value == nil) then value = "nil" end
  local valueType = type(value)
  if valueType ~= "string" and valueType ~= "number" and valueType ~= "boolean" then
    iup.MessageError(debugger.main_dialog, "Can edit only strings, numbers and booleans.")
    return
  end

  local status, newValue = iup.GetParam("Set Local", setparent_param_cb, name.." = ".."%s{true, false and nil are translated to Lua values.}\n", tostring(value))

  if (status) then
    local bol = string.lower(newValue)
    if bol == "true" then
      newValue = true
    elseif bol == "false" then
      newValue = false
    elseif bol == "nil" then
      newValue = nil
    else
      local num = tonumber(newValue)
      if num then
        newValue = num
      end
    end
  
    local list_stack = iup.GetDialogChild(debugger.main_dialog, "LIST_STACK")
    local index = tonumber(list_stack.value)

    -- here there are 4 levels on top of the stack: 
    --   1-DebuggerSetLocalVariable, 
    --   2-LoopStep
    --   3-DebuggerLineHook, 
    --   4-DebuggerHookFunction
    local startLevel = 5
    local level = index - 1 + startLevel  -- this is the level of the function
    
    iup.DebuggerSetLocalValue(tree_local, level, id, newValue)
    iup.DebuggerSetLocalTreeItem(tree_local, id, name, newValue)
  end
end

function iup.DebuggerPrintLocalVariable()
  local tree_local = iup.GetDialogChild(debugger.main_dialog, "TREE_LOCAL")
  local id = tree_local.value

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "PANEL_TABS")
  panelTabs.valuepos = 1 -- show console tab

  local userdata = iup.TreeGetUserId(tree_local, id)
  local pos = userdata.pos
  local value = userdata.value
  
  iup.ConsolePrint(tree_local["TITLE"..id] .. "  (pos="..pos..")")
  iup.ConsolePrintValue(value)
end

function iup.DebuggerPrintAllLocalVariables()
  local tree_local = iup.GetDialogChild(debugger.main_dialog, "TREE_LOCAL")
  local count = tonumber(tree_local.rootcount) -- print all at root only

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "PANEL_TABS")
  panelTabs.valuepos = 1 -- show console tab

  local id = 0
  for i = 0, count-1 do
    local userdata = iup.TreeGetUserId(tree_local, id)
    local pos = userdata.pos
    local value = userdata.value

    iup.ConsolePrint(tree_local["TITLE"..id] .. "  (pos="..pos..")")
    iup.ConsolePrintValue(value)

    id = tree_local["NEXT"..id]
  end
end

function iup.DebuggerSetTreeTableValue(tree, name, id, newValue)
  local parentId = tree["parent"..id]
  local parentUdata = iup.TreeGetUserId(tree, parentId)
  local parent = parentUdata.value
  parent[name] = newValue
end

function iup.DebuggerSetLocalValue(tree_local, level, id, newValue)
  local userdata = iup.TreeGetUserId(tree_local, id)
  local depth = tonumber(tree_local["depth"..id])

  if depth > 0 then
    iup.DebuggerSetTreeTableValue(tree_local, userdata.name, id, newValue)
  else
    local pos = userdata.pos
    local s = string.sub(userdata.name, 1, 3)
    if s == ":: " then
      debug.setupvalue(tree_local.func, pos, newValue)
    else
     level = level + 1 -- this is the level inside this function
     debug.setlocal(level, pos, newValue)
    end
  end

  userdata.value = newValue
end

--[[  UNUSED
function iup.DebuggerGetLocal(tree_local, level, id)
  local name, value
  local userdata = iup.TreeGetUserId(tree_local, id)
  local pos = userdata.pos
  local tree_value = tree_local["TITLE"..id]
  local s = string.sub(tree_value, 1, 3)
  if s == ":: " then
    name, value = debug.getupvalue(tree_local.func, pos)
  else
    level = level + 1 -- this is the level inside this function
    name, value = debug.getlocal(level, pos)
  end
  return name, value
end
]]

function iup.DebuggerLocalVariablesTreeAction(tree_local, id)
  local userdata = iup.TreeGetUserId(tree_local, id)
  local valueType = type(userdata.value)
  if valueType == "string" or valueType == "number" or valueType == "boolean" then
    iup.DebuggerSetDialogChildAttrib("SET_LOCAL", "ACTIVE", "Yes")
  else
    iup.DebuggerSetDialogChildAttrib("SET_LOCAL", "ACTIVE", "No")
  end
end

function iup.DebuggerLocalVariablesBranchOpenAction(tree_local, id)
  local userdata = iup.TreeGetUserId(tree_local, id)
  local tableValue = userdata.value
  local pos = userdata.pos
  
  if type(tableValue) ~= "table" or userdata.expanded then
    return
  end
  
  local dummy_id = id + 1
  userdata.expanded = true

  id = dummy_id

  -- the same algorithm used in ConsolePrintTable
  local name
  local tmp = {}
  for name, value in ipairs(tableValue) do 
    id = iup.DebuggerAddLocalTreeItem(tree_local, id, name, value, pos)
    tmp[name] = true
  end
  for name, value in pairs(tableValue) do 
    if (not tmp[name]) then
      id = iup.DebuggerAddLocalTreeItem(tree_local, id, name, value, pos)
    end
  end

  iup.DebuggerSetDialogChildAttrib("TREE_LOCAL", "DELNODE"..dummy_id, "SELECTED")
end

function iup.DebuggerGetTreeTitle(tree, id, name, value)
  local depth = tonumber(tree["depth"..id])
  local pref = ""
  local suff = ""
  if depth and depth > 0 then
    pref = "["
    suff = "]"
  end

  local title = pref .. name .. suff .. " = "
  
  if type(value) == "string" then
    title = title .. "\"" .. value .. "\""
  else
    title = title .. tostring(value)
  end
  
  title = title .. " <" .. type(value) .. ">"
  
  return title
end

function iup.DebuggerSetLocalTreeItem(tree_local, id, name, value)
  tree_local["TITLE"..id] = iup.DebuggerGetTreeTitle(tree_local, id, name, value)
end

function iup.DebuggerAddLocalTreeItem(tree_local, id, name, value, pos)
  local userdata = {}
  local lastAddedNode

  if type(value) == "table" then
    tree_local["INSERTBRANCH"..id] = iup.DebuggerGetTreeTitle(tree_local, id, name, value)
    lastAddedNode = tree_local.lastaddnode
    
    tree_local["ADDLEAF"..lastAddedNode] = "dummy"
    userdata.expanded = false
  else
    tree_local["INSERTLEAF"..id] = iup.DebuggerGetTreeTitle(tree_local, id, name, value)
    lastAddedNode = tree_local.lastaddnode
  end
  
  userdata.name = name
  userdata.value = value
  userdata.pos = pos
  
  iup.TreeSetUserId(tree_local, lastAddedNode, userdata)
    
  return tonumber(lastAddedNode)
end

function iup.DebuggerUpdateLocalVariablesTree(level)
  local name, value
  local pos
  local id = -1

  level = level + 1 -- this is the level inside this function

  iup.DebuggerClearLocalVariablesTree()

  local tree_local = iup.GetDialogChild(debugger.main_dialog, "TREE_LOCAL")

  pos = 1
  name, value = debug.getlocal(level, pos)
  while name ~= nil do
    if string.sub(name, 1, 1) ~= "(" then  -- do not include internal variables (loop control variables, temporaries, etc).
      id = iup.DebuggerAddLocalTreeItem(tree_local, id, name, value, pos)
    end

    pos = pos + 1
    name, value = debug.getlocal(level, pos)
  end

  -- vararg (only for Lua >= 5.2, ignored in Lua 5.1)
  pos = -1
  name, value = debug.getlocal(level, pos)
  while name ~= nil do
    name = "vararg[" .. -pos .. "]"
    id = iup.DebuggerAddLocalTreeItem(tree_local, id, name, value, pos)

    pos = pos - 1
    name, value = debug.getlocal(level, pos)
  end

  local call = debug.getinfo(level, "uf")
  if call.nups > 0 then
    pos = 1
    tree_local.func = call.func
    name, value = debug.getupvalue(call.func, pos)
    while name ~= nil do
      name = ":: " .. name
      id = iup.DebuggerAddLocalTreeItem(tree_local, id, name, value, pos)

      pos = pos + 1
      name, value = debug.getupvalue(call.func, pos)
    end
  else
    tree_local.func = nil
  end

  if (id > 0) then
    iup.DebuggerSetDialogChildAttrib("PRINT_LOCAL", "ACTIVE", "Yes")
    iup.DebuggerSetDialogChildAttrib("SET_LOCAL", "ACTIVE", "Yes")

    tree_local.value = 1 -- select first item on list
  end
end

function iup.DebuggerShowTip(word, line)

  if debugger.debugState ~= DEBUG_PAUSED and debugger.debugState ~= DEBUG_ERRORPAUSED then
    return
  end

  local list_stack = iup.GetDialogChild(debugger.main_dialog, "LIST_STACK")
  local index = tonumber(list_stack.value)

  -- here there are 4 levels on top of the stack: 
  --   1-DebuggerShowTip, 
  --   2-LoopStep
  --   3-DebuggerLineHook, 
  --   4-DebuggerHookFunction
  local startLevel = 5
  local level = index - 1 + startLevel  -- this is the level of the function

  -- check if inside the current function
  local info = debug.getinfo(level, "SlL") -- linedefined, lastlinedefined, activelines
  if info.linedefined == 0 and info.lastlinedefined == 0 then
    if not info.activelines[line] then
      return
    end
  elseif line < info.linedefined or line > info.lastlinedefined then
    return
  end

  -- check for local
  local pos = 1
  local name, value = debug.getlocal(level, pos)
  while name ~= nil do
    if name == word then
      return tostring(value)
    end

    pos = pos + 1
    name, value = debug.getlocal(level, pos)
  end

  -- check for upvalues
  local call = debug.getinfo(level, "uf")
  if call.nups > 0 then
    pos = 1
    name, value = debug.getupvalue(call.func, pos)
    while name ~= nil do
      if name == word then
        return tostring(value)
      end

      pos = pos + 1
      name, value = debug.getupvalue(call.func, pos)
    end
  end

  -- check for global  
  if _G[word] then
    return tostring(_G[word])
  end
end


------------------------------------- Stack -------------------------------------


function iup.DebuggerStackListAction(list_stack, index)
  
  -- here there are 4 levels on top of the stack: 
  --   1-DebuggerStackListAction, 
  --   2-LoopStep
  --   3-DebuggerLineHook, 
  --   4-DebuggerHookFunction
  local startLevel = 5
  local level = index - 1 + startLevel  -- this is the level of the function
  
  iup.DebuggerUpdateLocalVariablesTree(level)
  
  local filename = list_stack["FILENAME" .. index]
  local line = list_stack["LINE" .. index]

  iup.DebuggerSelectLine(filename, tonumber(line), false) -- do not find
end

function iup.DebuggerStackListActivate(list_stack, index)
  local startLevel = 5
  local level = index - 1 + startLevel  -- this is the level of the function
  
  iup.DebuggerUpdateLocalVariablesTree(level)
  
  local filename = list_stack["FILENAME" .. index]
  local line = list_stack["LINE" .. index]
  local source = list_stack["SOURCE" .. index]

  iup.DebuggerSelectLine(filename, tonumber(line), true, source) -- find
end

function iup.DebuggerClearStackList()
  iup.DebuggerSetDialogChildAttrib("PRINT_LEVEL", "ACTIVE", "NO")

  iup.DebuggerSetDialogChildAttrib("LIST_STACK", "REMOVEITEM", "ALL")
end

function iup.DebuggerPrintStackLevel()
  local list_stack = iup.GetDialogChild(debugger.main_dialog, "LIST_STACK")
  local index = list_stack.value

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "PANEL_TABS")
  panelTabs.valuepos = 1 -- show console tab

  local defined = list_stack["DEFINED" .. index]

  iup.ConsolePrint(list_stack[index] .. "  (level=" .. index..")")
  iup.ConsolePrint(defined)
end

function iup.DebuggerPrintAllStackLevel()
  local list_stack = iup.GetDialogChild(debugger.main_dialog, "LIST_STACK")
  local count = tonumber(list_stack.count)

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "PANEL_TABS")
  panelTabs.valuepos = 1 -- show console tab

  for index = 1, count do
    local defined = list_stack["DEFINED" .. index]

    iup.ConsolePrint(list_stack[index] .. "  (level=" .. index..")")
    iup.ConsolePrint(defined)
  end
end

function iup.DebuggerUpdateStackList()
  local info, desc, defined
  
  -- here there are 3 levels on top of the stack: 
  --   1-DebuggerUpdateStackList, 
  --   2-DebuggerLineHook, 
  --   3-DebuggerHookFunction
  local startLevel = 4
  local level = startLevel

  iup.DebuggerClearStackList()

  local list_stack = iup.GetDialogChild(debugger.main_dialog, "LIST_STACK")
  
  info = debug.getinfo(level)--, "Snl") -- source, name, namewhat, what, currentline, linedefined
  while  info ~= nil do
    if info.what == "main" then
      desc = "<main>"
    elseif info.name and info.name ~= "" then
      desc = info.name
    else
      desc = "<noname>"
    end
    if _VERSION ~= "Lua 5.1" then
      local call = debug.getinfo(level, "uf")
      local params = ""
      local pos = 1
      local name, value = debug.getlocal(call.func, pos)
      while name ~= nil do
        params = params .. name .. ", "
        pos = pos + 1
        name, value = debug.getlocal(call.func, pos)
      end
      if params ~= "" then 
        if call.isvararg then
          params = params .. "..."
        else
          params = string.sub(params, 1, -3) -- remove last ", "
        end
        desc = desc .. "(" .. params .. ")"
      end
    end
    if info.namewhat ~= "" then
        desc = desc .. " <".. info.namewhat .. ">"
    end
    if info.currentline > 0 then
       desc = desc .. " at line " .. info.currentline
    end

    local filename
    if info.what == "C" then    
      defined = "  [Defined in C"
    else
      local s = string.sub(info.source, 1, 1)
      if s == "@" then
        filename = string.sub(info.source, 2)
        defined = "  [Defined in the file: \"" .. filename .. "\""
      else
        filename = string.sub(info.short_src, 2, -2)
        defined = "  [Defined in a " .. filename
      end
    end
    if info.linedefined > 0 then
       defined = defined .. ", line " .. info.linedefined .. "]"
    else
       defined = defined .. "]"
    end

    local index = (level - startLevel) + 1
    list_stack[index] = desc
    list_stack["DEFINED" .. index] = defined
    list_stack["FILENAME" .. index] = filename
    list_stack["SOURCE" .. index] = info.source
    list_stack["LINE" .. index] = info.currentline

    level = level + 1

    info = debug.getinfo(level)--, "Snl") -- source, name, namewhat, what, currentline, linedefined
  end
  
  if level > startLevel then
    iup.DebuggerSetDialogChildAttrib("PRINT_LEVEL", "ACTIVE", "YES")

    list_stack.value = 1 -- select first item on list (startLevel)
    iup.DebuggerUpdateLocalVariablesTree(startLevel)
  end
  
end


----------------------------  Globals       --------------------------

function iup.DebuggerIsExpression(name)
  if string.find(name, "[^%w_]") then -- finds any non alphanumeric character, but allows underscore
    return true
  else
    return false
  end
end

function iup.DebuggerGetGlobalNameFromTree(tree_global, id)
  local userdata = iup.TreeGetUserId(tree_global, id)
  return userdata.globalname
end

function iup.DebuggerGetGlobalValue(name)
  local value
  -- used only for root nodes, and only when debuger is active
  
  if iup.DebuggerIsExpression(name) then
    local cmd, msg = loadstring("return " .. name)
    if not cmd then
      value = "<Invalid Expression> " .. msg
    else
      local result = {pcall(cmd)}
      if result[1] then
        value = result[2]
      else
        value = "<Invalid Expression>" .. result[2]
      end
    end
  else
    value = _G[name]
  end

  return value
end

function iup.DebuggerGlobalsTreeAction(tree_global, id)
  local userdata = iup.TreeGetUserId(tree_global, id)
  local name = userdata.globalname
  if iup.DebuggerIsExpression(name) then
    iup.DebuggerSetDialogChildAttrib("SET_GLOBAL", "ACTIVE", "NO")
  else
    iup.DebuggerSetDialogChildAttrib("SET_GLOBAL", "ACTIVE", "YES")
  end
end

function iup.DebuggerGlobalVariablesBranchOpenAction(tree_global, id)
  -- can not expand when debugger is not active  
  if not iup.DebuggerIsActive() then
    return iup.IGNORE
  end
  
  local userdata = iup.TreeGetUserId(tree_global, id)
  local tableValue = userdata.value
  
  if type(tableValue) ~= "table" or userdata.expanded then
    return
  end
  
  local dummy_id = id + 1
  userdata.expanded = true

  id = dummy_id

  -- the same algorithm used in ConsolePrintTable
  local name
  local tmp = {}
  for name, value in ipairs(tableValue) do 
    id = iup.DebuggerAddGlobalsTreeItem(tree_global, id, name, value)
    tmp[name] = true
  end
  for name, value in pairs(tableValue) do 
    if (not tmp[name]) then
      id = iup.DebuggerAddGlobalsTreeItem(tree_global, id, name, value)
    end
  end

  iup.DebuggerSetDialogChildAttrib("TREE_GLOBAL", "DELNODE"..dummy_id, "SELECTED")
end

function iup.DebuggerInitGlobalsTree(tree_global)
  if not tree_global then 
    tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")
  end

  local count = tonumber(tree_global.rootcount)

  local id = 0
  for i = 0, count-1 do
    local userdata = iup.TreeGetUserId(tree_global, id)
    local name = userdata.globalname
    tree_global["TITLE"..id] = name
    tree_global["IMAGE"..id] = "IMGEMPTY"
    tree_global["IMAGEEXPANDED"..id] = "IMGEMPTY"
    tree_global["DELNODE"..id] = "CHILDREN"

    id = tree_global["NEXT"..id]
  end

  tree_global.expandall = "NO"

  local count = tonumber(tree_global.rootcount)
  if (count > 0) then
    iup.DebuggerSetDialogChildAttrib("PRINT_GLOBAL", "ACTIVE", "Yes")
    iup.DebuggerSetDialogChildAttrib("SET_GLOBAL", "ACTIVE", "Yes")
    iup.DebuggerSetDialogChildAttrib("REMOVE_GLOBAL", "ACTIVE", "Yes")

    tree_global.value = 1 -- select first item on list
    iup.DebuggerGlobalsTreeAction(tree_global, tree_global.value)
  else
    iup.DebuggerSetDialogChildAttrib("PRINT_GLOBAL", "ACTIVE", "NO")
    iup.DebuggerSetDialogChildAttrib("SET_GLOBAL", "ACTIVE", "NO")
    iup.DebuggerSetDialogChildAttrib("REMOVE_GLOBAL", "ACTIVE", "NO")
  end
end

function iup.DebuggerUpdateGlobalsTree()
  -- this is called only during debug
  local tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")
  local count = tonumber(tree_global.rootcount) -- print all at root only

  -- clear all secondary branches (including "dummy")
  local id = 0
  for i = 0, count-1 do
    tree_global["DELNODE"..id] = "CHILDREN"
    id = tree_global["NEXT"..id]
  end
  
  id = 0
  for i = 0, count-1 do
    local userdata = iup.TreeGetUserId(tree_global, id)
    local name = userdata.globalname
    local value = iup.DebuggerGetGlobalValue(name)
    userdata.value = value
    userdata.expanded = false
    
    iup.DebuggerSetGlobalsTreeItem(tree_global, id, name, value)

    id = tree_global["NEXT"..id]
  end

  tree_global.expandall = "NO"
end

function iup.DebuggerSetGlobalsTreeItem(tree_global, id, name, value)
  tree_global["TITLE"..id] = iup.DebuggerGetTreeTitle(tree_global, id, name, value)
  
  if type(value) == "table" then
    tree_global["IMAGE"..id] = "IUP_treeplus"
    tree_global["IMAGEEXPANDED"..id] = "IUP_treeminus"

    -- restore "dummy"
    tree_global["ADDLEAF"..id] = "dummy"
  else
    tree_global["IMAGE"..id] = "IMGEMPTY"
    tree_global["IMAGEEXPANDED"..id] = "IMGEMPTY"
  end
end

function iup.DebuggerAddGlobalsTreeItem(tree_global, id, name, value)
  local userdata = {}
  local lastAddedNode
  
  if type(value) == "table" then
    tree_global["INSERTBRANCH"..id] = iup.DebuggerGetTreeTitle(tree_global, id, name, value)
    lastAddedNode = tree_global.lastaddnode
    tree_global["IMAGE"..lastAddedNode] = "IUP_treeplus"
    tree_global["IMAGEEXPANDED"..lastAddedNode] = "IUP_treeminus"
    
    tree_global["ADDLEAF"..lastAddedNode] = "dummy"
  else
    tree_global["INSERTBRANCH"..id] = iup.DebuggerGetTreeTitle(tree_global, id, name, value)
    lastAddedNode = tree_global.lastaddnode
    tree_global["IMAGE"..lastAddedNode] = "IMGEMPTY"
    tree_global["IMAGEEXPANDED"..lastAddedNode] = "IMGEMPTY"
  end
  
  userdata.globalname = name
  userdata.expanded = false
  userdata.value = value
  
  iup.TreeSetUserId(tree_global, lastAddedNode, userdata)
  
  return lastAddedNode
end

function iup.DebuggerAddGlobal(tree_global, name)
  local id = tonumber(tree_global.last0)
  if not id then id = -1 end
   
  if iup.DebuggerIsActive() then
    local value = iup.DebuggerGetGlobalValue(name)
    return iup.DebuggerAddGlobalsTreeItem(tree_global, id, name, value)
  else
    local userdata = {}
    
    userdata.globalname = name
    userdata.expanded = false
    
    tree_global["INSERTBRANCH"..id] = name
    local lastAddedNode = tree_global.lastaddnode
    tree_global["IMAGE"..lastAddedNode] = "IMGEMPTY"
    tree_global["IMAGEEXPANDED"..lastAddedNode] = "IMGEMPTY"
    
    iup.TreeSetUserId(tree_global, lastAddedNode, userdata)
    return lastAddedNode
  end
end

function iup.DebuggerAddGlobalVariable()
  local status, newName = iup.GetParam("Add Global", setparent_param_cb, "Name = ".."%s\n", "")
  if (status) then
    local tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")
    local count = tonumber(tree_global.rootcount)
   
    local lastAddedNode = iup.DebuggerAddGlobal(tree_global, newName)

    if (count == 0) then
      iup.DebuggerSetDialogChildAttrib("PRINT_GLOBAL", "ACTIVE", "Yes")
      iup.DebuggerSetDialogChildAttrib("SET_GLOBAL", "ACTIVE", "Yes")
      iup.DebuggerSetDialogChildAttrib("REMOVE_GLOBAL", "ACTIVE", "Yes")
    end
    
    iup.DebuggerGlobalsTreeAction(tree_global, lastAddedNode)
  end
end

function iup.DebuggerRemoveGlobalVariable()
  local tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")
  local id = tree_global.value
  if (not id or tonumber(id) < 0) then
    iup.MessageError(debugger.main_dialog, "Select a variable or expression on the list.")
    return
  end
  
  -- get the root item
  local parentId = tree_global["parent"..id]
  while parentId do
    id = parentId
    parentId = tree_global["parent"..id]
  end
  
  -- select the next item
  local next_id = tree_global["next"..id]
  if next_id then
    tree_global.value = next_id
  else
    local previous_id = tree_global["previous"..id]
    if previous_id then
      tree_global.value = previous_id
    end
  end

  tree_global["DELNODE"..id] = "SELECTED"

  local count = tonumber(tree_global.rootcount)
  if (count == 0) then
    iup.DebuggerSetDialogChildAttrib("PRINT_GLOBAL", "ACTIVE", "No")
    iup.DebuggerSetDialogChildAttrib("SET_GLOBAL", "ACTIVE", "No")
    iup.DebuggerSetDialogChildAttrib("REMOVE_GLOBAL", "ACTIVE", "No")
  else

    iup.DebuggerGlobalsTreeAction(tree_global, tree_global.value)
  end
end

function iup.DebuggerRemoveAllGlobalVariable()
  local tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")

  tree_global["DELNODE"] = "ALL"

  iup.DebuggerSetDialogChildAttrib("PRINT_GLOBAL", "ACTIVE", "No")
  iup.DebuggerSetDialogChildAttrib("SET_GLOBAL", "ACTIVE", "No")
  iup.DebuggerSetDialogChildAttrib("REMOVE_GLOBAL", "ACTIVE", "No")
end

function iup.DebuggerSetGlobalVariable()
  local tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")
  local id = tree_global.value
  if (not id or tonumber(id) < 0) then
    iup.MessageError(debugger.main_dialog, "Select a variable on the list.")
    return
  end
  
  local userdata = iup.TreeGetUserId(tree_global, id)
  
  local name = userdata.globalname

  if iup.DebuggerIsExpression(name) then
    iup.MessageError(debugger.main_dialog, "Can not change expressions values.")
    return
  end
  
  local value
  local depth = tonumber(tree_global["depth"..id])
  if depth > 0 then
    value = userdata.value
  else
    value = iup.DebuggerGetGlobalValue(name)
  end

  if (value == nil) then value = "nil" end
  local valueType = type(value)
  if valueType ~= "string" and valueType ~= "number" and valueType ~= "boolean" then
    iup.MessageError(debugger.main_dialog, "Can edit only strings, numbers and booleans.")
    return
  end

  local status, newValue = iup.GetParam("Set Global", setparent_param_cb, name.." = ".."%s{true, false and nil are translated to Lua values.}\n", tostring(value))

  if (status) then
    local bol = string.lower(newValue)
    if bol == "true" then
      newValue = true
    elseif bol == "false" then
      newValue = false
    elseif bol == "nil" then
      newValue = nil
    else
      local num = tonumber(newValue)
      if num then
        newValue = num
      end
    end

    if depth > 0 then
      iup.DebuggerSetTreeTableValue(tree_global, name, id, newValue)
    else
      _G[name] = newValue
    end

    if iup.DebuggerIsActive() then
      iup.DebuggerSetGlobalsTreeItem(tree_global, id, name, newValue)
    else
      tree_global["TITLE"..id] = name
      tree_global["IMAGE"..id] = "IMGEMPTY"
      tree_global["IMAGEEXPANDED"..id] = "IMGEMPTY"
    end
  end
end

function iup.DebuggerPrintGlobalVariable()
  local tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")
  local id = tree_global.value

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "CSUTOM_TABS")
  panelTabs.valuepos = 1 -- show console tab
  
  local userdata = iup.TreeGetUserId(tree_global, id)
  
  local value
  local depth = tonumber(tree_global["depth"..id])
  if depth > 0 then
    value = userdata.value
  else
    value = iup.DebuggerGetGlobalValue(userdata.globalname)
  end

  iup.ConsolePrint(tree_global["TITLE"..id])
  iup.ConsolePrintValue(value)
end

function iup.DebuggerPrintAllGlobalVariables()
  local tree_global = iup.GetDialogChild(debugger.main_dialog, "TREE_GLOBAL")
  local count = tonumber(tree_global.rootcount) -- print all at root only

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "PANEL_TABS")
  panelTabs.valuepos = 1 -- show console tab

  local id = 0
  for i = 0, count-1 do
    local userdata = iup.TreeGetUserId(tree_global, id)
    local value = iup.DebuggerGetGlobalValue(userdata.globalname)

    iup.ConsolePrint(tree_global["TITLE"..id])
    iup.ConsolePrintValue(value)

    id = tree_global["NEXT"..id]
  end
end


----------------------------  Debug State       --------------------------


function iup.DebuggerGetFuncLevel()
-- level 0 is the current function (getinfo itself); 
-- level 1 is the function that called getinfo (DebuggerGetFuncLevel)
  local func_level = 1
  repeat 
    func_level = func_level + 1
  until debug.getinfo(func_level, "l") == nil  -- only current line, default is all info
  return func_level - 1
end

function iup.DebuggerUpdateState(filename, line)
  if debugger.debugState == DEBUG_STEP_OUT then
    if debugger.stepFuncState == FUNC_STATE_OUTSIDE then
      iup.DebuggerSetState(DEBUG_PAUSED)
      
      debugger.stepFuncState = FUNC_STATE_OUTSIDE
      debugger.stepFuncLevel = debugger.currentFuncLevel
    end
  elseif debugger.debugState == DEBUG_STEP_INTO or
      (debugger.debugState == DEBUG_STEP_OVER and debugger.stepFuncState == FUNC_STATE_OUTSIDE) or
      (debugger.debugState ~= DEBUG_PAUSED and iup.DebuggerHasBreakpoint(filename, line)) then
      iup.DebuggerSetState(DEBUG_PAUSED)
  end
end

function iup.DebuggerErrorMessage(message)
  local dlg = iup.messagedlg
  {
    parentdialog = debugger.main_dialog,
    dialogtype = "ERROR",
    buttons = "OKCANCEL",
    title = "Lua Error!",
  }
  
  dlg.value = message .. "\n\nWould you like to debug at error location?"
    
  dlg:popup(iup.CENTERPARENT, iup.CENTERPARENT)
  local ret = dlg.buttonresponse
  dlg:destroy()

  if ret == "1" then
    return true
  else
    return false
  end
end

function iup.DebuggerForcePause(filename, currentline, source)
  iup.DebuggerSetState(DEBUG_ERRORPAUSED)

  iup.DebuggerHighlightLine(filename, currentline, source)

  iup.DebuggerUpdateGlobalsTree()

  iup.DebuggerUpdateStackList()

  while debugger.debugState == DEBUG_ERRORPAUSED do
    local ret = iup.LoopStep()
    if ret == iup.CLOSE then
      iup.ExitLoop() -- repost to MainLoop
    end
  end

  -- in any change of state, simply stop the debugger
  iup.DebuggerSetState(DEBUG_INACTIVE)
  iup.DebuggerCloseTemporary()
end

function iup.DebuggerTraceBack(msg)
  if not msg then
    return
  end

  -- hook is always at level 2 when called
  local info = debug.getinfo(2, "Sl") -- what, source, currentline
  local s = string.sub(info.source, 1, 1)
  local filename
  if s == "@" then
    filename = string.sub(info.source, 2)
  else
    filename = string.sub(info.short_src, 2, -2)
  end

  iup._TRACEBACK = nil
  debug.sethook() -- turns off the hook

-- NOT necessary
-- msg = debug.traceback(msg)

  iup.ConsolePrint("Lua Error:")
  iup.ConsolePrint(msg)

  if iup.DebuggerErrorMessage(msg) then
    iup.DebuggerForcePause(filename, info.currentline, info.source)
  end
end

function iup.DebuggerLineHook(filename, line, source)
  debugger.currentFuncLevel = iup.DebuggerGetFuncLevel()

  iup.DebuggerUpdateState(filename, line)
  
  if debugger.debugState == DEBUG_PAUSED then
  
    iup.DebuggerHighlightLine(filename, line, source)
    
    iup.DebuggerUpdateGlobalsTree()
    
    iup.DebuggerUpdateStackList()
    
    debug.sethook() -- turns off the hook
    
    while debugger.debugState == DEBUG_PAUSED do
      local ret = iup.LoopStep()
      if ret == iup.CLOSE then
        iup.ExitLoop() -- repost to MainLoop
      end
    end
    
    if iup.DebuggerIsActive() then
      debug.sethook(iup.DebuggerHookFunction, "lcr") -- restore the hook if still active
    end
  end
  
  if debugger.debugState == DEBUG_STOPPED then
    iup.DebuggerEndDebug(true)
  end
end

function iup.DebuggerCallHook()
  if debugger.debugState == DEBUG_STEP_OVER then
    if debugger.stepFuncLevel == 0 then
      local func_level = iup.DebuggerGetFuncLevel()
      debugger.stepFuncState = FUNC_STATE_INSIDE
      debugger.stepFuncLevel = func_level
    end
  end
end

function iup.DebuggerReturnHook(filename, what)
  if filename == debugger.startFile and what == "main" then
    iup.DebuggerSetState(DEBUG_INACTIVE)
  elseif debugger.debugState == DEBUG_STEP_OUT or debugger.debugState == DEBUG_STEP_OVER then
    local func_level = iup.DebuggerGetFuncLevel()
    if debugger.stepFuncLevel == func_level then
      debugger.stepFuncState = FUNC_STATE_OUTSIDE
      debugger.stepFuncLevel = 0
    end
  end
end

function iup.DebuggerHookFunction(event, line)
  -- how many levels we have before the hook was invoked?
  -- hook is always at level 2 when called
  -- Inside a hook, you can call getinfo with level 2 to get more information about the running function
  local info = debug.getinfo(2, "S") -- what, source
  local s = string.sub(info.source, 1, 1)
  local filename
  if s == "@" then
    filename = string.sub(info.source, 2)
  else
    filename = string.sub(info.short_src, 2, -2)
  end

  if iup.DebuggerIsActive() then
    if event == "call" then
      iup.DebuggerCallHook()
    elseif event == "return" then
      iup.DebuggerReturnHook(filename, info.what)
    elseif event == "line" then
      iup.DebuggerLineHook(filename, line, info.source)
    end
  end

  local ret = iup.LoopStep()
  if ret == iup.CLOSE then
    iup.ExitLoop() -- repost to MainLoop
  end
end

function iup.DebuggerStartDebug(filename)
  debugger.startFile = filename

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "PANEL_TABS")
  panelTabs.valuepos = 3 -- show debug tab

  iup.ConsolePrint("-- Debug start")

  iup.DebuggerSetState(DEBUG_ACTIVE)

  debug.sethook(iup.DebuggerHookFunction, "lcr")

  iup._TRACEBACK = iup.DebuggerTraceBack
end

function iup.DebuggerEndDebug(stop)
  iup._TRACEBACK = nil

  debug.sethook() -- turns off the hook

  iup.DebuggerSetState(DEBUG_INACTIVE)

  local panelTabs = iup.GetDialogChild(debugger.main_dialog, "PANEL_TABS")
  panelTabs.valuepos = 1 -- show console tab

  if stop then
    iup.ConsolePrint("-- Debug stop!")
    error() -- abort processing, no error message
  else
    iup.ConsolePrint("-- Debug finish")
  end

  iup.DebuggerCloseTemporary()
end

function iup.DebuggerExit()
  if iup.DebuggerIsActive() then
    iup.DebuggerEndDebug(true) -- make a stop
  end
end
