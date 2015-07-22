------------------------------------------------------------------------------
-- Tree class 
------------------------------------------------------------------------------
local ctrl = {
  nick = "tree",
  parent = iup.WIDGET,
  subdir = "elem",
  creation = "",
  callback = {
    selection_cb = "nn",
    multiselection_cb = "nn",  -- fake definition to be replaced by treefuncs module
    multiunselection_cb = "nn",  -- fake definition to be replaced by treefuncs module
    branchopen_cb = "n",
    branchclose_cb = "n",
    executeleaf_cb = "n",
    noderemoved_cb = "s",  -- fake definition to be replaced by treefuncs module
    rename_cb = "ns",
    showrename_cb = "n",
    rightclick_cb = "n",
    dragdrop_cb = "nnnn",
    togglevalue_cb = "nn",
  },
  extrafuncs = 1,
}

iup.TREEREFTABLE={}

function iup.TreeSetNodeAttributes(ih, id, attrs)
  for attr, val in pairs(attrs) do
    ih[attr..id] = val
  end
end
ctrl.SetNodeAttributes = iup.TreeSetNodeAttributes

function iup.TreeSetAncestorsAttributes(ih, ini, attrs)
  ini = ih["parent"..ini]
  local stack = {}
    while ini do
      table.insert(stack, 1, ini)
      ini = ih["parent"..ini]
    end
  for i = 1, #stack do
    iup.TreeSetNodeAttributes(ih, stack[i], attrs)
  end
end
ctrl.SetAncestorsAttributes = iup.TreeSetAncestorsAttributes

function iup.TreeSetDescendantsAttributes(ih, ini, attrs)
  local id = ini
  for i = 1, ih["childcount"..ini] do
    id = id+1
    iup.TreeSetNodeAttributes(ih, id, attrs)
    if ih["kind"..id] == "BRANCH" then
      id = iup.TreeSetDescendantsAttributes(ih, id, attrs)
    end
  end
  return id
end
ctrl.SetDescendantsAttributes = iup.TreeSetDescendantsAttributes

function iup.TreeSetAttributeHandle(ih, name, value)
   if iup.GetClass(value) == "iupHandle" then value = iup.SetHandleName(value) end
   iup.SetAttribute(ih, name, value)
end

-- must be after the branch has nodes
function iup.TreeSetState(ih, tnode, id)
  if tnode.state then iup.SetAttribute(ih, "STATE"..id, tnode.state) end
end

function iup.TreeSetNodeAttrib(ih, tnode, id)
  if tnode.color then iup.SetAttribute(ih, "COLOR"..id, tnode.color) end
  if tnode.titlefont then iup.SetAttribute(ih, "TITLEFONT"..id, tnode.titlefont) end
  if tnode.marked then iup.SetAttribute(ih, "MARKED"..id, tnode.marked) end
  if tnode.image then iup.TreeSetAttributeHandle(ih, "IMAGE"..id, tnode.image) end
  if tnode.imageexpanded then iup.TreeSetAttributeHandle(ih, "IMAGEEXPANDED"..id, tnode.imageexpanded) end
  if tnode.userid then iup.TreeSetUserId(ih, id, tnode.userid) end
end

function iup.TreeAddNodesRec(ih, t, id)
  if t == nil then return end
  local cont = #t
  while cont >= 0 do
    local tnode = t[cont]
    if type(tnode) == "table" then
      if tnode.branchname then
        iup.SetAttribute(ih, "ADDBRANCH"..id, tnode.branchname)
        iup.TreeSetNodeAttrib(ih, tnode, id+1)
        iup.TreeAddNodesRec(ih, tnode, id+1)
        iup.TreeSetState(ih, tnode, id+1)
      elseif tnode.leafname then
        iup.SetAttribute(ih, "ADDLEAF"..id, tnode.leafname)
        iup.TreeSetNodeAttrib(ih, tnode, id+1)
      end
    else
      if tnode then
        iup.SetAttribute(ih, "ADDLEAF"..id, tnode)
      end
    end
    cont = cont - 1
   end
end

function iup.TreeAddNodes(ih, t, id)
  if (not id) then
    id = 0  -- default is the root
    if t.branchname then iup.SetAttribute(ih, "TITLE0", t.branchname) end
    iup.TreeSetNodeAttrib(ih, t, 0)
  end
  iup.TreeAddNodesRec(ih, t, id)
  if (id == 0) then iup.TreeSetState(ih, t, 0) end
end
ctrl.AddNodes = iup.TreeAddNodes

-- defined in C
ctrl.GetId = iup.TreeGetId
ctrl.GetUserId = iup.TreeGetUserId
ctrl.SetUserId = iup.TreeSetUserId

-- backward compatibility
iup.TreeSetValue = iup.TreeAddNodes

function ctrl.createElement(class, param)
  return iup.Tree()
end

iup.RegisterWidget(ctrl)
iup.SetClass(ctrl, "iupWidget")
