require("iuplua")

NUM_COL = 26   -- A-Z=26
NUM_LIN = 100  -- 0-99=100

FORM_READY = 0  -- number is ready to use
FORM_CALC = 1   -- calculating
FORM_DIRTY = 2  -- need calc

ERR_NONE = 0
ERR_PARSE = 1
ERR_RECURSE = 2
ERR_DEPEND = 3

data = {}

function cellDataInit()
  for i = 1, NUM_LIN do
    data[i] = {}
    for j = 1, NUM_COL do
      data[i][j] = { parseError = 0, number = 0.0, isNumber = nil, calcFormula = 0, value = nil,
               operation = { type = 0, startLin = 0, startCol = 0, endLin = 0, endCol = 0},
               dependencies = {}, dependenciesCount = 0 }
    end
  end
end

function cellDataGetValue(lin, col)
  return data[lin][col].value
end

function cellDataIsFormula(lin, col)
  if data[lin][col].isNumber or (data[lin][col].value and not string.match(data[lin][col].value, "%s*=%s*")) then
    return nil
  else
    return 1
  end
end

function cellDataSetDependenciesDirty(lin, col)
  local cellData = data[lin][col]
  for i = 1, cellData.dependenciesCount do
    local dep_lin = cellData.dependencies[i].lin
    local dep_col = cellData.dependencies[i].col
    data[dep_lin][dep_col].calcFormula = FORM_DIRTY
  end
end

function cellDataIsNumber(lin, col)
  return data[lin][col].isNumber or ((data[lin][col].value and string.match(data[lin][col].value, "%s*=%s*") and data[lin][col].parseError == ERR_NONE))
end

function cellDataCalcFormula(lin, col)
  if data[lin][col].calcFormula == FORM_CALC then
    data[lin][col].parseError = ERR_RECURSE
    return
  end

  cellDataSetDependenciesDirty(lin, col)

  if data[lin][col].parseError == ERR_NONE then
    if data[lin][col].operation.type == OP_SUM then
      data[lin][col].calcFormula = FORM_CALC

      data[lin][col].number = cellDataSumCells(lin, col)

      if data[lin][col].parseError == ERR_NONE then
        data[lin][col].calcFormula = FORM_READY
      else
        data[lin][col].calcFormula = FORM_DIRTY
      end
    end
  end
end

function cellDataSumCells(lin, col)
  local total = 0
  for i = data[lin][col].operation.startLin, data[lin][col].operation.endLin do
    for j = data[lin][col].operation.startCol, data[lin][col].operation.endCol do
      if cellDataIsNumber(i, j) then
        total = total + cellDataGetNumber(i, j)
      elseif data[i][j].value then
        data[lin][col].parseError = ERR_DEPEND
        return 0
      end
    end
  end
  return total
end

function cellDataGetNumber(lin, col)
  if data[lin][col].value and string.match(data[lin][col].value, "%s*=%s*") and data[lin][col].calcFormula ~= FORM_READY then
    cellDataCalcFormula(lin, col)
  end

  return data[lin][col].number
end

function cellDataGetString(lin, col)
  -- called when not a number
  if data[lin][col].value and string.match(data[lin][col].value, "%s*=%s*") then
    local error = data[lin][col].parseError
    if error ~= ERR_NONE then
      if error == ERR_PARSE then
        return "Error Invalid Formula!"
      elseif error == ERR_RECURSE then
        return "Error Recursive Formula!"
      else -- ERR_DEPEND
        return "Error Dependency in Formula!"
      end
    end
  end

  return data[lin][col].value
end

function cellDataSetNumber(lin, col, value)
  data[lin][col].isNumber = 1
  data[lin][col].number = value
  data[lin][col].calcFormula = FORM_READY
  data[lin][col].parseError = ERR_NONE
  data[lin][col].value = nil
  cellDataSetDependenciesDirty(lin, col)
end

function cellDataParseFormula(lin, col)
  local formula = data[lin][col].value

  data[lin][col].parseError = ERR_PARSE

  local op, c1, l1, c2, l2 = string.match(formula, "%s*=%s*(%l+)%((%u)(%d):(%u)(%d)%)%s*")
  if not op or not c1 or not l1 or not c2 or not l2 then
    return
  end

  if op == "sum" then
    data[lin][col].operation.type = OP_SUM
  else
    return
  end

  if c1 < "A" or c1 > "Z" then
    return
  end
  if c2 < "A" or c2 > "Z" then
    return
  end
  if tonumber(l1) < 0 or tonumber(l1) > 99 then
    return
  end
  if tonumber(l2) < 0 or tonumber(l2) > 99 then
    return
  end

  data[lin][col].parseError = ERR_NONE
  
  data[lin][col].operation.startCol = string.byte(c1) - string.byte('A') + 1
  data[lin][col].operation.startLin = tonumber(l1)
  data[lin][col].operation.endCol = string.byte(c2) - string.byte('A') + 1
  data[lin][col].operation.endLin = tonumber(l2)
end

function cellDataAddDependencies(lin, col, dep_lin, dep_col)
  for i = 1, data[lin][col].dependenciesCount do
    if dep_lin == data[lin][col].dependencies[i].lin and 
      dep_col == data[lin][col].dependencies[i].col then
      return -- already in the list
    end
  end

  -- not found, add to the list
  data[lin][col].dependenciesCount = data[lin][col].dependenciesCount + 1
  data[lin][col].dependencies[data[lin][col].dependenciesCount] = {}
  data[lin][col].dependencies[data[lin][col].dependenciesCount].lin = dep_lin
  data[lin][col].dependencies[data[lin][col].dependenciesCount].col = dep_col
end

function cellDataRemoveDependencies(lin, col, dep_lin, dep_col)
  local index = 0
  for i = 1, data[lin][col].dependenciesCount do
    if dep_lin == data[lin][col].dependencies[i].lin and
      dep_col == data[lin][col].dependencies[i].col then
      index = i
      break -- found in the list
    end
  end

  if index == data[lin][col].dependenciesCount then
    return
  end

  -- remove from the list
  for i = index, data[lin][col].dependenciesCount - 1 do
    data[lin][col].dependencies[i] = data[lin][col].dependencies[i+1]
  end
  
  data[lin][col].dependenciesCount = data[lin][col].dependenciesCount - 1
end

function cellDataUpdateDependencies(lin, col)
  for i = 1, NUM_LIN do
    for j = 1, NUM_COL do
      if i >= data[lin][col].operation.startLin and i <= data[lin][col].operation.endLin and
        j >= data[lin][col].operation.startCol and j <= data[lin][col].operation.endCol then
        cellDataAddDependencies(i, j, lin, col)
      else
        cellDataRemoveDependencies(i, j, lin, col)
      end
    end
  end
end 

function cellDataSetString(lin, col, value)
  cellDataSetDependenciesDirty(lin, col)
  
  data[lin][col].isNumber = nil
  data[lin][col].number = 0
  data[lin][col].calcFormula = FORM_READY
  data[lin][col].parseError = ERR_NONE
  data[lin][col].value = value
  
  if string.match(data[lin][col].value, "%s*=%s*") then
    data[lin][col].calcFormula = FORM_DIRTY
    
    cellDataParseFormula(lin, col)
    
    if data[lin][col].parseError == ERR_NONE then
      cellDataUpdateDependencies(lin, col)
    end
  end
end

--************************************** Interface **************************************************

matrix = iup.matrix{numcol = NUM_COL, numlin = NUM_LIN, numcol_visible = 4, numlin_visible = 7,
          width0 = 15, height0 = 8, resizematrix = "YES"}

dlg = iup.dialog{iup.hbox{matrix}; title = "Cells", size = "300x150"}

cellDataInit()

function matrix:value_cb(lin, col)
  local text
  local editcell = self.editcell
  if editcell then
    local elin, ecol = string.match(editcell, "(%d):(%d)")
    if tonumber(elin) == lin and tonumber(ecol) == col and cellDataIsFormula(lin, col) then
      return cellDataGetValue(lin, col)
    end
  end

  if lin == 0 and col == 0 then
    return ""
  elseif lin == 0 then
    return string.format("%c", string.byte("A") + col - 1)
  elseif col == 0 then
    return string.format("%d", lin)
  end

  if cellDataIsNumber(lin, col) then
    local numberValue = cellDataGetNumber(lin, col)
    if not numberValue or not cellDataIsNumber(lin, col) then 
      text = cellDataGetString(lin, col)
    else
      text = string.format("%.2f", numberValue)
    end
  else
    text = cellDataGetString(lin, col)
  end
  
  return text
end

function matrix:value_edit_cb(lin, col, newValue)
  if string.find(newValue, "^[+-]?%d+$") then -- check if there is more text after the number (text may start with a number)
    cellDataSetNumber(lin, col, newValue)
  else
    cellDataSetString(lin, col, newValue)
  end
end

dlg:showxy( iup.CENTER, iup.CENTER )

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
