function fib(i, j)
	return i+j
end

local start = os.clock()

local sum = 0
for i = 0, 1000*10000-1, 1 do
	sum = sum + fib(i, i+1)
end
print(sum)

print(os.clock()-start, "\n")
