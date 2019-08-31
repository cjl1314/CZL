local start = os.clock()

local s
local a = "foo"
local b = "bar"
local c = "ok"
for i = 0, 1000000-1, 1 do
	s = a..b..c..i
end

print(os.clock()-start, "\n")
print(s, "\n");
