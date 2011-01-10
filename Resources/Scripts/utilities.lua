--- The functions in this file are helper functions.

--- Convert a list of strings/numbers into an table with those values as keys
-- Code from: http://www.lua.org/pil/11.5.html
function Set (list)
  local set = {}
  for _, l in ipairs(list) do set[l] = true end
  return set
end

--- Trim a string
function trim(s)
	return (s:gsub("^%s*(.-)%s*$", "%1"))
end

-- Wrap lines of text to a specified maximum width or 72 characters by default.
function linewrap(text, chars_per_line, do_justify)
	-- Note: this justify function only works properly for fixed-width fonts,
	-- but it does still offer a slight improvement for other fonts.
	local justify = function(the_line)
		local puffed = 0
		local done = false
		local new_line = the_line
		while done == false do
			local newer_line = string.gsub(new_line, "([a-z]) ", function(c)
				if string.len(the_line) + puffed < chars_per_line then
					puffed = puffed + 1
					return (c .. "  ")
				end
				done = true
				return (c .. " ")
			end)
			-- if there was no change or the change is too dramatic, stick with the previous version
			if newer_line == new_line or string.match(newer_line, "    ") then return new_line end
			new_line = newer_line
		end
		return new_line
	end

	if chars_per_line == nil then chars_per_line = 72 end
	local wrapped = ""
	local line = ""
	text = string.gsub(text, "\n\n", "\n__HARDWRAP__ ")
	string.gsub(text, "([^ \n]*)[ \n]*",
	   function(w)
	      local joined = string.format("%s %s", line, w)
              if line == "" then joined = w end
	      if string.len( joined ) <= chars_per_line and w ~= "__HARDWRAP__" then
		 line = joined
	      else
		 if(do_justify) then line = justify(line) end
	         if wrapped == "" then wrapped = line
	         else wrapped = string.format("%s\n%s%s", wrapped, line, (w == "__HARDWRAP__" and "\n" or "")) end
	         line = (w == "__HARDWRAP__" and "" or w)
	      end
	      return ""
	   end)
	wrapped = string.format("%s\n%s", wrapped, line)
	return wrapped
end


--- Calculate the Distance between two points
function distfrom( pt1_x,pt1_y, pt2_x,pt2_y)
	if pt1_x == nil or pt1_y == nil or pt2_x == nil or pt2_y == nil then return nil end
	x_diff = (pt1_x - pt2_x)
	y_diff = pt1_y - pt2_y
	return math.sqrt(x_diff*x_diff + y_diff*y_diff)
end

--- Create a FailureWindow
function NewFailureWindow(Title,Message)
	if FailureWindow ~= nil then return end
	local height = 100
	local width = 300
	FailureWindow= UI.newWindow(350, 350, width, height, Title,
		UI.newLabel(20,20,Message),
		UI.newButton(width/2-50, height-50, 100, 30, "OK", "FailureWindow:close(); FailureWindow = nil"))
end

function about(r) return math.random(r)-r/2 end

--- Convert coordinate to quadrant
function coordinateToQuadrant(x,y)
	qsize = 4096
	function c2q(z)
		return math.floor( (z+qsize)/(2*qsize))
	end
	return c2q(x),c2q(y)
end

function QuadrantToCoordinate(x,y)
	qwidth = 4096*2
	return x*qwidth, y*qwidth
end

function serialize (o)
	ret = ""
	if type(o) == "number" then
		
		ret = ret .. o
	elseif type(o) == "string" then
		ret = ret .. string.format("%q", o)
	elseif type(o) == "table" then
		ret = ret .. "{\n"
		for k,v in pairs(o) do
			ret = ret
			   .. "  ["
			   .. serialize(k)
			   .. "] = "
			   .. serialize(v)
			   .. ",\n"
		end
		ret = ret .. "}\n"
	else
		error("cannot serialize a " .. type(o))
	end
	return ret
end

function choose( array )
	return array[math.random(#array)]
end

-- Shuffle a table
-- http://rosettacode.org/wiki/Knuth_shuffle#Lua
function table.shuffle(t)
  local n = #t
  while n > 1 do
    local k = math.random(n)
    t[n], t[k] = t[k], t[n]
    n = n - 1
  end
 
  return t
end

