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

function linewrap(text, chars_per_line)
	if chars_per_line == nil then chars_per_line = 72 end
	local ret = ""
	for line =1,math.ceil( text:len() / chars_per_line ) do
		local partial = text:sub( (line-1)*chars_per_line, (line)*chars_per_line -1)
		ret = ret .. partial .. "\n"
	end
	return ret
end

--- Calculate the Distance between two points
function distfrom( pt1_x,pt1_y, pt2_x,pt2_y)
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

