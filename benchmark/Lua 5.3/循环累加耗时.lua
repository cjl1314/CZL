local start = os.clock()

local sum = 0
local j = 0
for i = 0, 10000000-1, 1 do
	sum = sum + i + j
	j = j + 1
end
print(sum, "\n");

print(os.clock()-start, "\n")
