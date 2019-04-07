local start = os.clock()

local t1 = {}
for i = 1, 1000000, 1 do
	t1[i] = {1, 2, 3, 4, 5, 6}
end
print(os.clock()-start, "\n")

for i = 1, 100, 2 do --由于Lua删除元素速度非常慢，这里只删除100个
	table.remove(t1, i)
end
print(os.clock()-start, "\n")

local t2 = {}
for i = 1, 1000000, 1 do
	t2[i] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}
end
print(os.clock()-start, "\n")
