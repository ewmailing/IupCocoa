-- main

local s = [[
function f4(z)
  z = z * 100
  return z
end ]]

local func = loadstring(s) -- if chuckname is used source is the chunckname not the string
func()

local r = f4(3)
print(r)
