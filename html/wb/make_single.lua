
local files = {}
local file
local subdir

function get_filename(v)
  if v then
    -- only up to "#"
    local j = string.find(v, "#")
    if j then
      f = string.sub(v, 0, j-1)
    else
      f = v
    end
  
    if (not files[f]) then
      files[f] = true
      return f
    end
  end
  return nil
end

function writeheader(title)
  local header_part1 = [[
  <!doctype HTML PUBLIC "-//W3C//DTD HTML 4.0 Strict//EN">
  <html>
  <head>
  <meta http-equiv="Content-Language" content="en-us">
  <title>]]

  local header_part2 = [[</title>
  <link rel="stylesheet" type="text/css" href="../style.css">
  </style>
  </head>
  <body>]]

  file:write(header_part1 .. title .. header_part2)
end

function replace_navigation(body_data)
  local navigation = "(<div id=\"navigation\">.*</div>)"
  body_data, c = string.gsub(body_data, navigation, "")
  return body_data
end

function writeend()
  file:write("</body>\n")
  file:write("</html>\n")
end

function get_filepath(filename)
  local path_end
  local p = 1
  while (p) do
    p = string.find(filename, "/", p, true)
    if (p) then 
      path_end = p 
      p = p + 1
    end
  end
  if (not path_end) then
    return ""
  else
    local path = string.sub(filename, 1, path_end)
    return path
  end
end

function append_file(filename)
  print("  " .. filename)
  local input = io.open(filename, "r")
  if (not input) then
    print("File not found:\n  "..filename)
    return
  end
  local input_data = input:read("*a")
  
  -- extract body contents
  local body_start = string.find(input_data, "<body>")
  local body_end = string.find(input_data, "</body>")
  if not body_start then
    error("<body> not found.")
  end
  if not body_end then
    error("</body> not found.")
  end
  local body_data = string.sub(input_data, body_start+6, body_end+7)
  
  -- fix image path
  if (not string.find(body_data, "src=\"http")) then
    local filepath = get_filepath(filename)
    local repl = "%1" .. filepath
    body_data = string.gsub(body_data, "(src=\")", repl)
  end
  
  body_data = replace_navigation(body_data)
  
  file:write(body_data)
  input:close()
end

function writefolder(tree, parent_link)
  if (not tree) then
    return
  end
  local i = 1
  local n = #tree
  while i <= n do
    writetopic(tree[i], parent_link)
    i = i + 1
  end
end

function writetopic(t, parent_link)
  local link
  local f

  if t.link and t.link ~= "" then
    link = t.link
  else
    link = nil
  end
 
  local f = get_filename(link)
  if (f) then
    append_file(subdir..f)
  end

  -- Write folder --
  if t.folder then
    if link == nil then
      writefolder(t.folder, parent_link)
    else
      writefolder(t.folder, link)
    end
  end

end


dofile("wb_usr.lua")

if (not arg[1]) then
  error("Missing language parameter.")
end
              
-- lng -> from the command line
local lng = arg[1]
subdir = "../"..lng.."/"

local filename = wb_usr.file_title..".html"
print("Writing: " .. filename)
file = io.open(filename, "w")
writeheader(wb_usr.messages[lng].title)
writetopic(wb_usr.tree, nil)
writeend()
file:close()

print("done.")
