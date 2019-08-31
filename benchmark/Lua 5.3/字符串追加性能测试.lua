local start = os.clock()

local s
local a = {}
for i = 0, 1000000-1, 1 do
	a[0] = "foo"
	a[1] = "bar"
	a[2] = "ok"
	a[3] = i
	s = table.concat(a)
end

print(os.clock()-start, "\n")
print(s, "\n");
