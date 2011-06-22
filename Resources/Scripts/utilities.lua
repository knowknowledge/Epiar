-- The functions in this file are helper functions.

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

	-- These are the widths for FreeSans (See Resources/Blueprints/glyph-widths.sh if they need to be regenerated)
	local glyphWidths = {
		[' '] = 277, ['!'] = 277, ['('] = 333, [')'] = 333, [','] = 277, ['-'] = 333, ['.'] = 277, ['0'] = 555, ['1'] = 555, 
		['2'] = 555, ['3'] = 555, ['4'] = 555, ['5'] = 555, ['6'] = 555, ['7'] = 555, ['8'] = 555, ['9'] = 555, ['A'] = 666, 
		['B'] = 666, ['C'] = 722, ['D'] = 722, ['E'] = 666, ['F'] = 611, ['G'] = 777, ['H'] = 722, ['I'] = 277, ['J'] = 500, 
		['K'] = 666, ['L'] = 555, ['M'] = 833, ['N'] = 722, ['O'] = 777, ['P'] = 666, ['Q'] = 777, ['R'] = 722, ['S'] = 666, 
		['T'] = 611, ['U'] = 722, ['V'] = 666, ['W'] = 944, ['X'] = 666, ['Y'] = 666, ['Z'] = 611, ['a'] = 555, ['b'] = 277, 
		['c'] = 500, ['d'] = 555, ['e'] = 555, ['f'] = 277, ['g'] = 555, ['h'] = 555, ['i'] = 222, ['j'] = 333, ['k'] = 500, 
		['l'] = 222, ['m'] = 833, ['n'] = 555, ['o'] = 555, ['p'] = 555, ['q'] = 333, ['r'] = 333, ['s'] = 500, ['t'] = 277, 
		['u'] = 555, ['v'] = 333, ['w'] = 722, ['x'] = 500, ['y'] = 500, ['z'] = 333,
		['default'] = 500
	}

	local width_sum = function(text)
		local sum = 0
		string.gsub(text, "(.)", function(ch)
			local w = glyphWidths[ch]
			if w == nil then w = glyphWidths['default'] end
			sum = sum + w
		end)
		return sum
	end

	-- This justify function attempts to do the best job it can using the above width information,
	-- but it would probably be better if this kind of thing were handled at the widget level.
	local justify = function(the_line)
		local puffed = 0
		local done = false
		local new_line = the_line
		while done == false do
			local newer_line = string.gsub(new_line, "([a-z]) ", function(c)
				if ( width_sum(the_line) + glyphWidths[' ']*puffed ) / glyphWidths['default'] < chars_per_line then
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
	      --if string.len( joined ) <= chars_per_line and w ~= "__HARDWRAP__" then
	      if width_sum(joined) / glyphWidths['default'] <= chars_per_line and w ~= "__HARDWRAP__" then
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


-- Generic variable printing
-- Very useful for discovering the structure of Lua tables
-- http://lua-users.org/wiki/TableSerialization

function table_print (tt, indent, done)
  done = done or {}
  indent = indent or 4
  if type(tt) == "table" then
    local sb = {}
    for key, value in pairs (tt) do
      table.insert(sb, string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
        table.insert(sb, string.format("[%s] => ", tostring (key)));
        table.insert(sb, "{\n");
        table.insert(sb, table_print (value, indent + 2, done))
        table.insert(sb, string.rep (" ", indent)) -- indent it
        table.insert(sb, "}\n");
      elseif "number" == type(key) then
        table.insert(sb, string.format("\"%s\"\n", tostring(value)))
      else
        table.insert(sb, string.format(
            "%s = \"%s\"\n", tostring (key), tostring(value)))
       end
    end
    return table.concat(sb)
  else
    return tt .. "\n"
  end
end

function to_string( tbl )
    if  "nil"       == type( tbl ) then
        return tostring(nil)
    elseif  "table" == type( tbl ) then
        return table_print(tbl)
    elseif  "string" == type( tbl ) then
        return tbl
    else
        return tostring(tbl)
    end
end

