IUPTREE = {parent = WIDGET}
IUPTREEREFERENCETABLE = {} -- Used in C, see luatree.c

function IUPTREE:CreateIUPelement ()
  return iupCreateTree ()
end

function iuptree (o)
  return IUPTREE:Constructor (o)
end
iup.tree = iuptree

function TreeSetValueRec(handle, t, id)

  if t == nil then return end

  local cont = getn(t)

  while cont >= 0 do
    if type (t[cont]) == "table" then  
      if t[cont].branchname ~= nil then
        IupSetAttribute(handle, "ADDBRANCH"..id, t[cont].branchname)
      else
        IupSetAttribute(handle, "ADDBRANCH"..id, "")
      end
      TreeSetValueRec(handle, t[cont], id+1)
    else
      if t[cont] then
        IupSetAttribute(handle, "ADDLEAF"..id, t[cont])
      end
    end
    cont = cont - 1
   end 
end

function TreeSetValue(handle, t)
  if type(t) ~= "table" then
    IupMessage("TreeLua Error", "Incorrect arguments to function TreeSetValue")
    return
  end
  if t.branchname ~= nil then
    IupSetAttribute(handle, "NAME", t.branchname)
  end
  TreeSetValueRec(handle, t, 0)
end
iup.TreeSetValue = TreeSetValue

iup_callbacks.selection      = {"SELECTION_CB", iup_tree_selection_cb}
iup_callbacks.multiselection = {"MULTISELECTION_CB", iup_tree_multiselection_cb}
iup_callbacks.multiunselection = {"MULTIUNSELECTION_CB", iup_tree_multiunselection_cb}
iup_callbacks.branchopen     = {"BRANCHOPEN_CB", iup_tree_branchopen_cb}
iup_callbacks.branchclose    = {"BRANCHCLOSE_CB", iup_tree_branchclose_cb}
iup_callbacks.executeleaf    = {"EXECUTELEAF_CB", iup_tree_executeleaf_cb}
iup_callbacks.renamenode     = {"RENAMENODE_CB", iup_tree_renamenode_cb}
iup_callbacks.renamecb       = {"RENAME_CB", iup_tree_renamecb_cb}
iup_callbacks.showrenamecb   = {"SHOWRENAME_CB", iup_tree_showrenamecb_cb}
iup_callbacks.rightclick     = {"RIGHTCLICK_CB", iup_tree_rightclick_cb}
iup_callbacks.dragdrop       = {"DRAGDROP_CB", iup_tree_dragdrop_cb}

iup_callbacks.selection_cb      = iup_callbacks.selection      
iup_callbacks.multiselection_cb = iup_callbacks.multiselection 
iup_callbacks.multiunselection_cb = iup_callbacks.multiunselection 
iup_callbacks.branchopen_cb     = iup_callbacks.branchopen     
iup_callbacks.branchclose_cb    = iup_callbacks.branchclose    
iup_callbacks.executeleaf_cb    = iup_callbacks.executeleaf    
iup_callbacks.renamenode_cb     = iup_callbacks.renamenode     
iup_callbacks.rename_cb         = iup_callbacks.renamecb       
iup_callbacks.showrename_cb     = iup_callbacks.showrenamecb   
iup_callbacks.rightclick_cb     = iup_callbacks.rightclick     
iup_callbacks.dragdrop_cb       = iup_callbacks.dragdrop       
