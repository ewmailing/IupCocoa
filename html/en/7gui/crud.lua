require("iuplua")

data = {}
lastId = 0

function crud_create(surname, name)
	local item = {}
	lastId = lastId + 1
	item.name = name
	item.surname = surname
	item.id = lastId
	data[item.id] = item
	return lastId
end

function crud_read(nextId)
	local n, v = next(data, nextId)
	if not n then
		return n
	end
	return n, v.surname, v.name
end

function crud_update(id, surname, name)
	data[id].name = name
	data[id].surname = surname
end

function crud_delete(id)
	data[id] = nil
end

--********************************** Main *****************************************

lbl_filter = iup.label{title = "Filter prefix:"}
txt_filter = iup.text{size = "60x", expand = "HORIZONTAL"}
lst_names = iup.list{size = "60x", expand = "YES", visiblelines=6}
lbl_name = iup.label{title = "Name:", size = 35}
txt_name = iup.text{size = 60, expand = "HORIZONTAL"}
lbl_surname = iup.label{title = "Surname:", size = 35}
txt_surname = iup.text{size = 60, expand = "HORIZONTAL"}

btn_create = iup.button{title = "Create", size = "30"}
btn_update = iup.button{title = "Update", size = "30", active = "NO"}
btn_delete = iup.button{title = "Delete", size = "30", active = "NO"}

hbx_filter = iup.hbox{iup.hbox{lbl_filter, txt_filter; alignment = "ACENTER"}, iup.fill{}; homogeneous = "YES"}
hbx_name = iup.hbox{lbl_name, txt_name; alignment = "ACENTER"}
hbx_surname = iup.hbox{lbl_surname, txt_surname; alignment = "ACENTER"}
hbx_buttons = iup.hbox{btn_create, btn_update, btn_delete}

vbx_input = iup.vbox{hbx_name, hbx_surname}

hbox = iup.hbox{lst_names, vbx_input; homogeneous = "YES"}
box = iup.vbox{hbx_filter, hbox, hbx_buttons; nmargin = "10x10"}

dlg = iup.dialog{box, title = "CRUD", gap = "10"}

function trim (str)
return string.gsub(str, "^%s*(.-)%s*$", "%1")
end

function filterName(name, filter)
	local i,j = string.find(string.lower(name), string.lower(filter))
	return i==1
end

function loadList()
	local currId = nil
	local surname, name
	local count = 1
	local filter = txt_filter.value
	if filter then
		filter = trim(filter)
		if filter == "" then
			filter = nil
		end
	end
	lst_names[1] = nil
	currId, surname, name = crud_read(currId)
	while currId do
		if not filter or filterName(surname, filter) then
			lst_names[count] = surname..", "..name
			lst_names["ID"..count] = currId
			count = count + 1
		end
			currId, surname, name = crud_read(currId)
	end
	lst_names.value = 0
end

function updateButtonsState()
	if lst_names.value == "0" then
		btn_delete.active = "NO"
		btn_update.active = "NO"
	else
		btn_delete.active = "YES"
		btn_update.active = "YES"
	end
end

function lst_names:valuechanged_cb()
	updateButtonsState()
end

function txt_filter:valuechanged_cb()	
	loadList(self.value)
end

function btn_create:action()
	local name = trim(txt_name.value)
	local surname = trim(txt_surname.value)
	crud_create(surname, name)
	txt_name.value = ""
	txt_surname.value = ""
	loadList()
	updateButtonsState()
end

function btn_update:action()
	local index = lst_names.value
	local id = lst_names["ID"..index]
  	local name = trim(txt_name.value)
	local surname = trim(txt_surname.value)
	crud_update(tonumber(id), surname, name)
	txt_name.value = ""
	txt_surname.value = ""
	loadList()
	updateButtonsState()
end

function btn_delete:action()
	local index = lst_names.value
	local id = lst_names["ID"..index]
	crud_delete(tonumber(id), surname, name)
	txt_name.value = ""
	txt_surname.value = ""
	loadList()
	updateButtonsState()
end

dlg:showxy( iup.CENTER, iup.CENTER )

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
