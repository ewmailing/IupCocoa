
-- #################################################################################
--                               Widgets
-- #################################################################################


-- "type" is used to check the type of each parameter in the creation table
WIDGET = {type = {}}

-- called by the iupxxx functions
-- obj is a lua table
function WIDGET:Constructor(obj)
  -- the parent of the table is the widget class used to create the control
  obj.parent = self
  
  -- check the table parameters
  self:checkParams(obj)

  -- create the IUP control, calling iupCreateXXX
  obj.handle = self:CreateIUPelement(obj)

  -- set the parameters that are attributes
  self:setAttributes(obj)

  -- save the table indexed by the handle
  iup_handles[obj.handle] = obj

  -- the returned value is the handle, not the table
  return obj.handle
end

function WIDGET:checkParams (obj)
  local type = self.type
  local param, func = next(type, nil)
  while param do
    if not func(obj[param]) then
      error("parameter " .. param .. " has wrong value or is not initialized")
    end
    param, func = next(type, param)
  end
end

function WIDGET:setAttributes (obj)
  local temp = {}
  local f = next(obj, nil)
  while f do
    temp[f] = 1
    f = next(obj, f)
  end
  f = next(temp, nil)
  while f do
    obj:set (f, obj[f])
    f = next(temp, f)
  end
end

function WIDGET:get(index)
  if type_string (index) then
    if (iup_callbacks[index]) then
      return self[index]
    else  
      local INDEX = strupper (index)
      local value = IupGetAttribute (self.handle, INDEX)
      if value then
        local handle = IupGetHandle (value)
        if handle then
          return handle
        else
          return value
        end
      end
    end  
  end
  return self[index]
end

function WIDGET:set(index, value)
  if type_string (index) then
    local INDEX = strupper (index)
    local cb = iup_callbacks[index]
    
    -- workaround for resize attribute in dialog  
    if (index == "resize" and IupGetClassName(self.handle) == "dialog") then
      cb = nil
    end
   
    if (cb) then
      local func = cb[2]
      if (not func) then
        func = cb[IupGetClassName(self.handle)]
      end
      iupSetCallback(self.handle, cb[1], func, value)
      self[index] = value
      return
    elseif type_string(value) or type_number(value) then
      IupSetAttribute(self.handle, INDEX, value)
      return
    elseif type_nil(value) then
       local old_value = IupGetAttribute(self.handle, INDEX)
       if old_value then
          IupSetAttribute(self.handle, INDEX, value)
          return
       end
    elseif type_widget(value) then
      iupSetName(value)
      IupSetAttribute(self.handle, INDEX, value.IUP_name)
      return
    end
  end
  self[index] = value
end

function WIDGET:r_destroy()
  local i = 1
  local elem = self[i]
  while elem do
    if type_widget (elem) and elem.IUP_parent then
      if elem.IUP_parent == self then
        elem.IUP_parent = nil
        elem:r_destroy ()
      else    -- wrong parent
        error ("Internal table inconsistency")
        exit()
      end
    end

    i = i + 1
    elem = self[i]
  end
  iup_handles[self] = nil
end

function WIDGET:destroy()
  self:r_destroy ()
  IupDestroy (self)
end

function WIDGET:detach()
  IupDetach (self)
  local parent = self.IUP_parent
  if parent then
    self.IUP_parent = nil
    local i = 1
    while parent[i] do
      if parent[i] == self then
        while parent[i+1] do
        parent[i] = parent[i+1]
        i = i+1
        end
        parent[i] = nil
        return
      end
      i = i+1
    end
  end
end

function WIDGET:append(o)
  if IupAppend (self, o) then
    o.IUP_parent = self
    local i = 1
    while self[i] do
      if self[i] == o then
        return i
      end
      i = i+1
    end
    iup_handles[self][i] = o
    return i
  else
    return nil
  end
end

function WIDGET:map()
  return IupMap(self)
end

function WIDGET:hide()
  return IupHide(self)
end


-- ###############
IUPTIMER = {parent = WIDGET}

function IUPTIMER:CreateIUPelement ()
  return iupCreateTimer()
end

function iuptimer(o)
  return IUPTIMER:Constructor(o)
end
iup.timer = iuptimer


-- ###############
IUPCLIPBOARD = {parent = WIDGET}

function IUPCLIPBOARD:CreateIUPelement ()
  return iupCreateClipboard()
end

function iupclipboard(o)
  return IUPCLIPBOARD:Constructor(o)
end
iup.clipboard = iupclipboard


-- ###############
IUPDIALOG = {parent = WIDGET, type = {type_widget}}

function IUPDIALOG:CreateIUPelement (obj)
  local handle = iupCreateDialog(obj[1])
  if (obj[1]) then obj[1].IUP_parent = handle end
  return handle
end

function IUPDIALOG:show ()
  return IupShow(self)
end

function IUPDIALOG:showxy (x,y)
  return IupShowXY(self, x, y)
end

function IUPDIALOG:popup (x, y)
  return IupPopup (self, x, y)
end

function iupdialog (o)
  return IUPDIALOG:Constructor (o)
end
iup.dialog = iupdialog


-- ###############
IUPRADIO = {parent = WIDGET, type = {type_widget}}

function IUPRADIO:CreateIUPelement (obj)
  local handle = iupCreateRadio (obj[1])
  if (obj[1]) then obj[1].IUP_parent = handle end
  return handle
end

function iupradio (o)
  local handle = IUPRADIO:Constructor (o)
  iupCreateChildrenNames (handle[1])
  return handle
end
iup.radio = iupradio

-- OLD STUFF
function edntoggles (h)
  local tmp = {}
  local i = 1
  while h[i] do
    if type_string (h[i]) then
      tmp[i] = iuptoggle{title = h[i], action = h.action}
    else
      error ("option "..i.." must be a string")
    end
    i = i + 1
  end

  if h.value then
    local j = 1
    while h[j] and (h[j] ~= h.value) do
      j = j + 1
    end
    if h[j] then
      tmp.value = tmp[j]
    end
  elseif h.nvalue then
    tmp.value = tmp[h.nvalue]
  end

  return tmp
end

-- OLD STUFF
function edhradio (o)
  local toggles = edntoggles (o)
  return iupradio{edhbox (toggles); value = toggles.value}
end

-- OLD STUFF
function edvradio (o)
  local toggles = edntoggles (o)
  return iupradio{edvbox (toggles); value = toggles.value}
end


-- ###############
IUPMENU = {parent = WIDGET}

function IUPMENU:checkParams (obj)
  local i = 1
  while obj[i] do
    local o = obj[i]
    if not type_item (o) then   -- not a menu item
      if type (o) ~= 'table' then
        error("parameter " .. i .. " is not a table nor a menu item")
      elseif (o[1] and not type_string (o[1])) then
        error("parameter " .. i .. " does not have a string title")
      elseif (o[2] and not type_string (o[2]) and not type_function (o[2])
              and not type_widget (o[2])) then
        error("parameter " .. i .. " does not have an action nor a menu")
      end
    end
    i = i + 1
  end
end

function IUPMENU:CreateIUPelement (obj)
  local handle = iupCreateMenu ()
  local i = 1
  while obj[i] do
    local o = obj[i]
    local elem
    if type_widget (o) then  -- predefined
      elem = o
    elseif not o[1] then     -- Separator
      elem = iupseparator {}
    elseif type_widget (o[2]) then    -- SubMenu
      o.title = o[1]
      o[1] = o[2]
      o[2] = nil
      elem = iupsubmenu(o)
    else          -- Item
      o.title = o[1]
      o.action = o[2]
      o[1] = nil
      o[2] = nil
      elem = iupitem(o)
    end
    IupAppend (handle, elem)
    elem.IUP_parent = handle
    obj[i] = elem
    i = i + 1
  end
  return handle
end

function iupmenu (o)
  return IUPMENU:Constructor (o)
end
iup.menu = iupmenu

function IUPMENU:popup (x, y)
  return IupPopup (self, x, y)
end


-- ###############
COMPOSITION = {parent = WIDGET}

function COMPOSITION:checkParams (obj)
  local i = 1
  while obj[i] do
    if not type_widget (obj[i]) then
      error("parameter " .. i .. " has wrong value or is not initialized")
    end
    i = i + 1
  end
end

function COMPOSITION:CreateIUPelement (obj)
  local handle = self:CreateBoxElement ()
  local filled = obj.filled
  local i = 1
  local n = 0
  while obj[i] do
    n = n + 1
    i = i + 1
  end
  i = 1

  if filled == IUP_YES then 
    obj[i+n] = iupfill{}
    IupAppend (handle, obj[i+n])
    obj[i+n].IUP_parent = handle
  end

  while i <= n do
    IupAppend (handle, obj[i])
    obj[i].IUP_parent = handle
    i = i + 1
    if filled == IUP_YES then 
      obj[i+n] = iupfill{}
      IupAppend (handle, obj[i+n])
      obj[i+n].IUP_parent = handle
    end
  end
  return handle
end


-- ###############
IUPHBOX = {parent = COMPOSITION}

function IUPHBOX:CreateBoxElement ()
  return iupCreateHbox ()
end

function iuphbox (o)
  return IUPHBOX:Constructor (o)
end
iup.hbox = iuphbox

-- OLD STUFF
function edhbox (o)
  o.filled = IUP_YES
  return IUPHBOX:Constructor (o)
end

-- OLD STUFF
function edfield (f)
  local l, t
  if (type_string (f.prompt) or type_number (f.prompt)) then
    l = iuplabel {title = f.prompt}
  else
    error ("parameter prompt has wrong value or is not initialized")
  end
  if f.value then
    t = iuptext {value = f.value}
  else
    t = iuptext {value = f.nvalue}
  end
  if t and l then
    return edhbox {l, t}
  else
    return nil
  end
end


-- ###############
IUPVBOX = {parent = COMPOSITION}

function IUPVBOX:CreateBoxElement ()
  return iupCreateVbox ()
end

function iupvbox (o)
  return IUPVBOX:Constructor (o)
end
iup.vbox = iupvbox

-- OLD STUFF
function edvbox (o)
  o.filled = IUP_YES
  return IUPVBOX:Constructor (o)
end


-- ###############
IUPZBOX = {parent = COMPOSITION}

function IUPZBOX:CreateBoxElement ()
  return iupCreateZbox ()
end

function iupzbox (obj)
  local handle = IUPZBOX:Constructor (obj)
  local i = 1
  while obj[i] do
    iupSetName(handle[i])
    i = i+1
  end
  return handle
end
iup.zbox = iupzbox


-- ###############
IUPFILL = {parent = WIDGET}

function IUPFILL:CreateIUPelement ()
  return iupCreateFill ()
end

function iupfill (o)
  return IUPFILL:Constructor (o)
end
iup.fill = iupfill


-- ###############
IUPBUTTON = {parent = WIDGET, type = {title = type_string}}

function IUPBUTTON:CreateIUPelement (obj)
  if not obj.title and obj.image then
    obj.title=''
  end
  return iupCreateButton(obj.title)
end

function iupbutton (o)
  return IUPBUTTON:Constructor (o)
end
iup.button = iupbutton


-- ###############
IUPTEXT = {parent = WIDGET}

function IUPTEXT:CreateIUPelement ()
  return iupCreateText()
end

function iuptext (o)
  return IUPTEXT:Constructor (o)
end
iup.text = iuptext


-- ###############
IUPMULTILINE = {parent = IUPTEXT}

function IUPMULTILINE:CreateIUPelement ()
  return iupCreateMultiLine()
end

function iupmultiline (o)
  return IUPMULTILINE:Constructor (o)
end
iup.multiline = iupmultiline


-- ###############
IUPLABEL = {parent = WIDGET, type = {title = type_string}}

function IUPLABEL:CreateIUPelement (obj)
  if not obj.title and obj.image then
    obj.title=''
  end
  return iupCreateLabel (obj.title)
end

function iuplabel (o)
  return IUPLABEL:Constructor (o)
end
iup.label = iuplabel


-- ###############
IUPTOGGLE = {parent = IUPBUTTON}

function IUPTOGGLE:CreateIUPelement (obj)
  return iupCreateToggle (obj.title)
end

function iuptoggle (o)
  return IUPTOGGLE:Constructor (o)
end
iup.toggle = iuptoggle


-- ###############
IUPITEM = {parent = IUPBUTTON}

function IUPITEM:CreateIUPelement (obj)
  return iupCreateItem (obj.title)
end

function iupitem (o)
  return IUPITEM:Constructor (o)
end
iup.item = iupitem


-- ###############
IUPSUBMENU = {parent = WIDGET, type = {type_menu; title = type_string}}

function IUPSUBMENU:CreateIUPelement (obj)
  local h = iupCreateSubmenu (obj.title, obj[1])
  obj[1].IUP_parent = h
  return h
end

function iupsubmenu (o)
  return IUPSUBMENU:Constructor (o)
end
iup.submenu = iupsubmenu


-- ###############
IUPSEPARATOR = {parent = WIDGET}

function IUPSEPARATOR:CreateIUPelement ()
  return iupCreateSeparator ()
end

function iupseparator (o)
  return IUPSEPARATOR:Constructor (o)
end
iup.separator = iupseparator


-- ###############
IUPFILEDLG = {parent = WIDGET}

function IUPFILEDLG:popup (x, y)
  return IupPopup (self, x, y)
end

function IUPFILEDLG:CreateIUPelement ()
  return iupCreateFileDlg ()
end

function iupfiledlg (o)
  return IUPFILEDLG:Constructor (o)
end
iup.filedlg = iupfiledlg


-- ###############
IUPMESSAGEDLG = {parent = WIDGET}

function IUPMESSAGEDLG:popup (x, y)
  return IupPopup (self, x, y)
end

function IUPMESSAGEDLG:CreateIUPelement ()
  return iupCreateMessageDlg ()
end

function iupmessagedlg (o)
  return IUPMESSAGEDLG:Constructor (o)
end
iup.messagedlg = iupmessagedlg


-- ###############
IUPCOLORDLG = {parent = WIDGET}

function IUPCOLORDLG:popup (x, y)
  return IupPopup (self, x, y)
end

function IUPCOLORDLG:CreateIUPelement ()
  return iupCreateColorDlg ()
end

function iupcolordlg (o)
  return IUPCOLORDLG:Constructor (o)
end
iup.colordlg = iupcolordlg


-- ###############
IUPFONTDLG = {parent = WIDGET}

function IUPFONTDLG:popup (x, y)
  return IupPopup (self, x, y)
end

function IUPFONTDLG:CreateIUPelement ()
  return iupCreateFontDlg ()
end

function iupfontdlg (o)
  return IUPFONTDLG:Constructor (o)
end
iup.fontdlg = iupfontdlg


-- ###############
IUPUSER = {parent = WIDGET}

function IUPUSER:CreateIUPelement ()
  return iupCreateUser ()
end

function iupuser ()
  return IUPUSER:Constructor ()
end
iup.user = iupuser


-- ###############
IUPNORMALIZER = {parent = WIDGET}

function IUPNORMALIZER:checkParams (obj)
  local i = 1
  while obj[i] do
    if not type_widget (obj[i]) then
      error("parameter " .. i .. " has wrong value or is not initialized")
    end
    i = i + 1
  end
end

function IUPNORMALIZER:CreateIUPelement (obj)
  local handle = iupCreateNormalizer ()
  local i = 1
  while obj[i] do
    handle.addcontrol = obj[i]
    i = i + 1
  end
  return handle
end

function iupnormalizer ()
  return IUPNORMALIZER:Constructor ()
end
iup.normalizer = iupnormalizer


-- ###############
IUPFRAME = {parent = WIDGET, type = {type_widget}}

function IUPFRAME:CreateIUPelement (obj)
  local h = iupCreateFrame (obj[1])
  if (obj[1]) then obj[1].IUP_parent = h end
  return h
end

function iupframe (o)
  return IUPFRAME:Constructor (o)
end
iup.frame = iupframe


-- ###############
IUPCANVAS = {parent = WIDGET}

function IUPCANVAS:CreateIUPelement ()
  return iupCreateCanvas ()
end

function iupcanvas (o)
  return IUPCANVAS:Constructor (o)
end
iup.canvas = iupcanvas


-- ###############
IUPLIST = {parent = WIDGET}

function IUPLIST:CreateIUPelement ()
  return iupCreateList ()
end

function IUPLIST:get(index)
  if type (index) == 'number' then
    return IupGetAttribute (self.handle, ""..index)
  else
    return WIDGET.get(self, index)
  end
end

function IUPLIST:set (index, value)
  if type (index) == 'number' then
    if (type_string (value) or type_number (value)) then
      return IupSetAttribute (self.handle, ""..index, ""..value)
    elseif value == nil then
      return IupSetAttribute (self.handle, ""..index, value)
    end
  end
  return WIDGET.set(self, index, value)
end

function iuplist (o)
  return IUPLIST:Constructor (o)
end
iup.list = iuplist


-- ###############
IUPIMAGE = {parent = WIDGET}

function IUPIMAGE:checkParams (obj)
  local i = 1
  while obj[i] do
    local j = 1
    while obj[i][j] do
      if type (obj[i][j]) ~= 'number' then
        error ("non-numeric value in image definition")
      end
      j = j + 1
    end

    if obj.width and (j - 1) ~= obj.width then
      error ("inconsistent image lenght")
    else
      obj.width = j - 1
    end

    i = i + 1
  end
  
  obj.height = i - 1
end

function IUPIMAGE:CreateIUPelement (obj)
  local handle = iupCreateImage (obj.width, obj.height, obj)
  if type (obj.colors) == 'table' then
    local i = 1
    while obj.colors[i] do
      IupSetAttribute (handle, i, obj.colors[i])
      i = i + 1
    end
  end
  return handle
end

function iupimage (o)
  return IUPIMAGE:Constructor (o)
end
iup.image = iupimage


IUPIMAGERGB = {parent = WIDGET}

function IUPIMAGERGB:CreateIUPelement (obj)
  return iupCreateImageRGB(obj.width, obj.height, obj.pixels)
end

function iupimagergb (o)
  return IUPIMAGERGB:Constructor (o)
end
iup.imagergb = iupimagergb


IUPIMAGERGBA = {parent = WIDGET}

function IUPIMAGERGBA:CreateIUPelement (obj)
  return iupCreateImageRGBA(obj.width, obj.height, obj.pixels)
end

function iupimagergba (o)
  return IUPIMAGERGBA:Constructor (o)
end
iup.imagergba = iupimagergba


-- ###############
IUPPROGRESSBAR = {parent = WIDGET}

function IUPPROGRESSBAR:CreateIUPelement ()
  return iupCreateProgressBar()
end

function iupprogressbar (o)
  return IUPPROGRESSBAR:Constructor (o)
end
iup.progressbar = iupprogressbar


-- #################################################################################
--                                     Callbacks
-- #################################################################################


-- global list of callbacks
-- index is the Lua callback name
-- each callback contains the full name, and the C callback
iup_callbacks = 
{
  action      = {"ACTION", nil},
  actioncb    = {"ACTION_CB", nil},
  getfocus    = {"GETFOCUS_CB", iup_getfocus_cb},
  killfocus   = {"KILLFOCUS_CB", iup_killfocus_cb},
  focus       = {"FOCUS_CB", iup_focus_cb},
  k_any       = {"K_ANY", iup_k_any},
  help        = {"HELP_CB", iup_help_cb},
  caretcb     = {"CARET_CB", iup_caret_cb},
  keypress    = {"KEYPRESS_CB", iup_keypress_cb},
  scroll      = {"SCROLL_CB", iup_scroll_cb},
  trayclick   = {"TRAYCLICK_CB", iup_trayclick_cb},
  close       = {"CLOSE_CB", iup_close_cb},
  copydata    = {"COPYDATA_CB", iup_copydata_cb},
  open        = {"OPEN_CB", iup_open_cb},
  showcb      = {"SHOW_CB", iup_show_cb},
  mapcb       = {"MAP_CB", iup_map_cb},
  unmapcb     = {"UNMAP_CB", iup_unmap_cb},
  destroycb   = {"DESTROY_CB", iup_destroy_cb},
  dropfiles   = {"DROPFILES_CB", iup_dropfiles_cb},
  menuclose   = {"MENUCLOSE_CB", iup_menuclose_cb},
  highlight   = {"HIGHLIGHT_CB", iup_highlight_cb},
  wom         = {"WOM_CB", iup_wom_cb},
  wheel       = {"WHEEL_CB", iup_wheel_cb},
  button      = {"BUTTON_CB", iup_button_cb},
  resize      = {"RESIZE_CB", iup_resize_cb},
  move      = {"RESIZE_CB", iup_move_cb},
  motion      = {"MOTION_CB", iup_motion_cb},
  enterwindow = {"ENTERWINDOW_CB", iup_enterwindow_cb},
  leavewindow = {"LEAVEWINDOW_CB", iup_leavewindow_cb},
  edit        = {"EDIT_CB", iup_edit_cb},
  multiselect = {"MULTISELECT_CB", iup_multiselect_cb},
  filecb      = {"FILE_CB", iup_file_cb},
  mdiactivatecb = {"MDIACTIVATE_CB", iup_mdiactivate_cb},
  dropdowncb = {"DROPDOWN_CB", iup_dropdown_cb},
  dblclickcb = {"DBLCLICK_CB", iup_dblclick_cb},
}

iup_callbacks.action.toggle = iup_action_toggle
iup_callbacks.action.multiline = iup_action_text
iup_callbacks.action.text = iup_action_text
iup_callbacks.action.button = iup_action_button
iup_callbacks.action.list = iup_action_list
iup_callbacks.action.item = iup_action_button
iup_callbacks.action.canvas = iup_action_canvas

-- must set here because it is also used elsewhere with a different signature
iup_callbacks.actioncb.timer = iup_action_timer

-- aliases for the full names
iup_callbacks.action_cb      = iup_callbacks.actioncb    
iup_callbacks.getfocus_cb    = iup_callbacks.getfocus    
iup_callbacks.killfocus_cb   = iup_callbacks.killfocus   
iup_callbacks.focus_cb       = iup_callbacks.focus       
iup_callbacks.k_any          = iup_callbacks.k_any       
iup_callbacks.help_cb        = iup_callbacks.help        
iup_callbacks.caret_cb       = iup_callbacks.caretcb     
iup_callbacks.keypress_cb    = iup_callbacks.keypress    
iup_callbacks.scroll_cb      = iup_callbacks.scroll      
iup_callbacks.trayclick_cb   = iup_callbacks.trayclick   
iup_callbacks.close_cb       = iup_callbacks.close       
iup_callbacks.copydata_cb    = iup_callbacks.copydata       
iup_callbacks.open_cb        = iup_callbacks.open        
iup_callbacks.show_cb        = iup_callbacks.showcb      
iup_callbacks.map_cb         = iup_callbacks.mapcb       
iup_callbacks.unmap_cb       = iup_callbacks.unmapcb       
iup_callbacks.destroy_cb     = iup_callbacks.destroycb       
iup_callbacks.dropfiles_cb   = iup_callbacks.dropfiles   
iup_callbacks.menuclose_cb   = iup_callbacks.menuclose   
iup_callbacks.highlight_cb   = iup_callbacks.highlight   
iup_callbacks.wom_cb         = iup_callbacks.wom         
iup_callbacks.wheel_cb       = iup_callbacks.wheel       
iup_callbacks.button_cb      = iup_callbacks.button      
iup_callbacks.resize_cb      = iup_callbacks.resize      
iup_callbacks.move_cb        = iup_callbacks.move
iup_callbacks.motion_cb      = iup_callbacks.motion      
iup_callbacks.enterwindow_cb = iup_callbacks.enterwindow 
iup_callbacks.leavewindow_cb = iup_callbacks.leavewindow 
iup_callbacks.edit_cb        = iup_callbacks.edit        
iup_callbacks.multiselect_cb = iup_callbacks.multiselect 
iup_callbacks.mdiactivate_cb = iup_callbacks.mdiactivatecb
iup_callbacks.file_cb        = iup_callbacks.filecb 
iup_callbacks.dropdown_cb    = iup_callbacks.dropdowncb 
iup_callbacks.dblclick_cb    = iup_callbacks.dblclickcb 
iup_callbacks.valuechanged_cb = iup_callbacks.valuechangedcb 
                     