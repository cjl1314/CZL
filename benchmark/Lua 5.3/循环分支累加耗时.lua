local start = os.clock()

local sum = 0
for i = 0, 10000000-1, 1 do
	if i <= 2500000 then
		sum = sum+i
	elseif i <= 5000000 then
		sum = sum+i+1
	elseif i <= 7500000 then
		sum = sum+i+2
	else
		sum = sum+i+3
	end
end
print(sum, "\n");

print(os.clock()-start, "\n")
