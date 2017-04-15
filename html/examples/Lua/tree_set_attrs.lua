require"iuplua"

if (not loadstring) then
  loadstring = load
end

local nodes = {
	branchname = "root (0)",
	state = "EXPANDED",
	{
		branchname = "1.1 (1)",
		state = "EXPANDED",
		{
			branchname = "1.1.1 (2)",
			state = "EXPANDED",
			{
				branchname = "1.1.1.1 (3)",
				state = "EXPANDED",
				"1.1.1.1.1 (4)",
				"1.1.1.1.2 (5)",
			},
			{
				branchname = "1.1.1.2 (6)",
				state = "EXPANDED",
				"1.1.1.2.1 (7)",
				"1.1.1.2.2 (8)",
			},
		},
		{
			branchname = "1.1.2 (9)",
			state = "EXPANDED",
			{
				branchname = "1.1.2.1 (10)",
				state = "EXPANDED",
				"1.1.2.1.1 (11)",
				"1.1.2.1.2 (12)",
			},
			{
				branchname = "1.1.2.2 (13)",
				state = "EXPANDED",
				"1.1.2.2.1 (14)",
				"1.1.2.2.2 (15)",
			},
		},
	},
	{
		branchname = "1.2 (16)",
		state = "EXPANDED",
		{
			branchname = "1.2.1 (17)",
			state = "EXPANDED",
			{
				branchname = "1.2.1.1 (18)",
				state = "EXPANDED",
				"1.2.1.1.1 (19)",
				"1.2.1.1.2 (20)",
			},
			{
				branchname = "1.2.1.2 (21)",
				state = "EXPANDED",
				"1.2.1.2.1 (22)",
				"1.2.1.2.2 (23)",
			},
		},
		{
			branchname = "1.2.2 (24)",
			state = "EXPANDED",
			{
				branchname = "1.2.2.1 (25)",
				state = "EXPANDED",
				"1.2.2.1.1 (26)",
				"1.2.2.1.2 (27)",
			},
			{
				branchname = "1.2.2.2 (28)",
				state = "EXPANDED",
				"1.2.2.2.1 (29)",
				"1.2.2.2.2 (30)",
			},
		},
	},
}

local tree = iup.tree{
	map_cb = function(self)
		--iup.TreeAddNodes(self, nodes)
		self:AddNodes(nodes)
	end,
}

local no = iup.text{}
local attrs = iup.text{
	value = "{ color = '255 0 0', }",
	size = "200x",
	getvalue = function(self)
		return loadstring("return "..self.value)()
	end,
}

dlg = iup.dialog{
	iup.vbox{
		tree,
		iup.hbox{
			iup.label{ title = "Node:", },
			no,
			iup.label{ title = "Attributes:", },
			attrs,
		},
		iup.hbox{
			iup.button{
				title = "Ancestors",
				action = function()
					--iup.TreeSetAncestorsAttributes(tree, no.value, attrs:getvalue())
					tree:SetAncestorsAttributes(no.value, attrs:getvalue())
				end,
			},
			iup.button{
				title = "Descendants",
				action = function()
					--iup.TreeSetDescendantsAttributes(tree, no.value, attrs:getvalue())
					tree:SetDescendantsAttributes(no.value, attrs:getvalue())
				end,
			},
			iup.button{
				title = "All",
				action = function()
					for node = 0, tree.count-1 do
						--iup.TreeSetNodeAttributes(tree, node, attrs:getvalue())
						tree:SetNodeAttributes(node, attrs:getvalue())
					end
				end,
			},
		},
    gap = 5,
    margin = "10x10",
	},
}

dlg:show()
tree.value = 15
no.value = 15

if (iup.MainLoopLevel()==0) then
  iup.MainLoop()
end
