require("iuplua")


--*********************************** Circle List ****************************************/

DEF_RADIUS = 30
MAX_RADIUS = 100
circlesList = {}
circlesCount = 0
lastId = 1

function circles_add(x, y)
	circlesCount = circlesCount + 1
	circlesList[circlesCount] = {}
	circlesList[circlesCount].x = x
	circlesList[circlesCount].y = y
	circlesList[circlesCount].r = DEF_RADIUS
	circlesList[circlesCount].id = lastId  -- each circle has an unique id
	lastId = lastId + 1
	return lastId - 1
end

function circles_insert(id, x, y, r)
	circlesCount = circlesCount + 1
	circlesList[circlesCount] = {}
	circlesList[circlesCount].x = x
	circlesList[circlesCount].y = y
	circlesList[circlesCount].r = r
	circlesList[circlesCount].id = id
	return 1
end

function circles_remove(id)
	local index
	for i = 1, circlesCount do
		if circlesList[i].id == id then
			index = i
			break
		end
	end
	for j = index, circlesCount - 1 do
		circlesList[j] = circlesList[j + 1]
	end
	circlesCount = circlesCount - 1
end

function circles_update(id, r)
	local index
	for i = 1, circlesCount do
		if circlesList[i].id == id then
			index = i
			break
		end
	end
	circlesList[index].r = r
end

function circles_pick(x, y)
	local min_r = MAX_RADIUS
	local id = -1
	for i = 1, circlesCount do
		local dx = circlesList[i].x - x
		local dy = circlesList[i].y - y
		local r = dx*dx + dy*dy
		r = r^0.5
		if r < circlesList[i].r then
			if id < 0 then
				id = circlesList[i].id
				min_r = r
			elseif r < min_r then
				id = circlesList[i].id
			end
		end
	end
	return id
end

function circles_find(id)
	local index
	for i = 1, circlesCount do
		if circlesList[i].id == id then
			index = i
			break
		end
	end
	return circlesList[index]
end

--************************************* Operation List ************************************************/

MAX_OPERATIONS = 200

OP_CREATE = 1
OP_REMOVE = 2
OP_UPDATE = 3

operationsList = {}
operationsCount = 0
currentOperation = 0

function op_add(op_type, id, x, y, r)
	if currentOperation < operationsCount then
		operationsCount = currentOperation
	end
	operationsCount = operationsCount + 1
	operationsList[operationsCount] = {}
	operationsList[operationsCount].type = op_type
	operationsList[operationsCount].x = x
	operationsList[operationsCount].y = y
	operationsList[operationsCount].r = r
	operationsList[operationsCount].id = id
	currentOperation = operationsCount
end

function op_undo(void)
	if currentOperation < 1 then
		return
	end

	local op = operationsList[currentOperation]
	currentOperation = currentOperation - 1

	if op.type == OP_CREATE then
		circles_remove(op.id)
	elseif op.type == OP_REMOVE then
		circles_insert(op.id, op.x, op.y, op.r)
	elseif op.type == OP_UPDATE then
		op = operationsList[currentOperation]
		currentOperation = currentOperation - 1 -- return 2 to get the old value 

		circles_update(op.id, op.r)
	end
end

function op_redo(void)
	if currentOperation == operationsCount then
		return
	end

	currentOperation = currentOperation + 1
	op = operationsList[currentOperation]

	if op.type == OP_CREATE then
		circles_insert(op.id, op.x, op.y, op.r)
	elseif op.type == OP_REMOVE then
		circles_remove(op.id)
	elseif op.type == OP_UPDATE then
		currentOperation = currentOperation + 1 -- increment 2 to get the new value
		op = operationsList[currentOperation]

		circles_update(op.id, op.r)
	end
end

function op_has_undo()
	if currentOperation < 1 then
		return nil
	end
	return 1
end

function op_has_redo()
	if currentOperation == operationsCount then
		return nil
	end
	return 1
end


--*********************************************** Interface ****************************************/

function update_buttons()
	if op_has_undo() then
		undoButton.active = "YES"
	  else
		undoButton.active = "NO"
	end

	if op_has_redo() then
		redoButton.active = "YES"
	  else
		redoButton.active = "NO"
	end
end

undoButton = iup.button{title = "Undo", size = "60", active = "NO"}
redoButton = iup.button{title = "Redo", size = "60", active = "NO"}

canvas = iup.canvas{expand = "YES", highlightedCircle = -1}

hbox = iup.hbox{iup.fill{}, undoButton, redoButton, iup.fill{}}

vbox = iup.vbox{hbox, canvas;  nmargin = "10x10", gap = "10"}

dlg = iup.dialog{vbox, title = "Circle Drawer", size = "300x150"}

val_lbl = iup.label{"Adjust the diameter of the circle at (100, 100)"; expand = "HORIZONTAL"}

val = iup.val{"HORIZONTAL"; expand = "HORIZONTAL", min = "0", max = MAX_RADIUS}

config_box = iup.vbox{iup.fill{}, val_lbl, val, iup.fill{}, alignment = "ACENTER", nmargin = "10x10"}

config_dial = iup.dialog{config_box; parentdialog = dlg, title = "Circle Config"}

function undoButton:action()
	op_undo()
	update_buttons()
	iup.Update(canvas)
end

function redoButton:action()
	op_redo()
	update_buttons()
	iup.Update(canvas)
end

function canvas:action(posx, posy)
	iup.DrawBegin(self)
	local w, h = iup.DrawGetSize(self)
	self.drawcolor = "255 255 255"
	self.drawstyle = "FILL"
	iup.DrawRectangle(self, 0, 0, w - 1, h - 1)
	self.drawcolor = "0 0 0"
	self.drawstyle = "STROKE"
	local highlightedCircle = tonumber(self.highlightedcircle)
	for i = 1, circlesCount do
    local circle = circlesList[i]
		if highlightedCircle == circle.id then
			self.drawcolor = "128 128 128"
			self.drawstyle = "FILL"
		else
			self.drawcolor = "0 0 0"
			self.drawstyle = "STROKE"
		end
		iup.DrawArc(self, circle.x - circle.r, circle.y - circle.r, 
                      circle.x + circle.r, circle.y + circle.r, 0., 360.)
	end
	iup.DrawEnd(self)
end

function canvas:button_cb(button, pressed, x, y, status)
	if button == iup.BUTTON1 and pressed == 1 then
		local id = circles_pick(x, y)
		if id > 0 then
			self.highlightedcircle = id
		else
			id = circles_add(x, y)
			local circle = circles_find(id)
			op_add(OP_CREATE, circle.id, circle.x, circle.y, circle.r)
			update_buttons()
		end
		iup.Update(self)
	elseif button == iup.BUTTON3 and pressed == 1 then
		local highlightedCircle = tonumber(self.highlightedcircle)
		local id = circles_pick(x, y)
		if id == highlightedCircle then
			local circle = circles_find(id)
			local old_r = circle.r
			local new_r = old_r
			val_lbl.title = string.format("Adjust the diameter of the circle at (%d, %d)", circle.x, circle.y)
			val.value = old_r
			config_dial.circle_id = id
			iup.Popup(config_dial, iup.CENTERPARENT, iup.CENTERPARENT)
			new_r = tonumber(val.value)
			if new_r ~= old_r then
				op_add(OP_UPDATE, circle.id, circle.x, circle.y, old_r) -- add both old and new
				op_add(OP_UPDATE, circle.id, circle.x, circle.y, new_r)
				update_buttons()
			end
		end
	end
end

function val:valuechanged_cb()
	local circle_id = tonumber(config_dial.circle_id)
	local r = tonumber(self.value)
	circles_update(circle_id, r)
	iup.Update(canvas)
end

dlg:showxy( iup.CENTER, iup.CENTER )

if (iup.MainLoopLevel()==0) then
	iup.MainLoop()
end
