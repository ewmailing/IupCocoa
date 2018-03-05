-- main

local l_main_x = 33
g_main_x = 44

local t = {1, "xx", _G}
g = {123, "yy", t}

for i = 1, 10 do
  l_main_x = l_main_x + 1
  g_main_x = g_main_x - 1
end

local function f2(z)
  z = z + 1
  return z
end

function f1(x, ...)
  local y = x
  local ta = {...} -- vararg expansion
  x = "Hello " .. x .. " " .. l_main_x -- upvalue l_main_x
  y = false
  if y then
    y = 2 + g_main_x
  end
  y = f2(3) -- upvalue f2
  return x .. " " .. y
end

function f3()
  g_main_x = g_main_x + 50
  l_main_x = l_main_x + 5
end

local ret = f1("IUP", 3, "x23")
print(ret)

f3()

-- to test pause
-- while g_main_x do
  -- g_main_x = g_main_x + 1
-- end
-- 
g_main_x = g_main_x + 1


--local file = io.open("stripter_test2.lua", "rb")
--if file then 
--    s = file:read "*a"
--    file:close()
--end
--print(s)

--require("stripter_test2")

--f_load = loadfile("stripter_test2.lua")
--f_load()

--dofile("stripter_test2.lua")

--f4()

