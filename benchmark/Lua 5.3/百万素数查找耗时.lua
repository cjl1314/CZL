local total = 0
function is_prime(n) 
	local count = math.sqrt(n)+1
    for x = 2, count do
        if (n % x) == 0 then 
			return false 
		end
    end
	total = total + 1
    return true
end

local start = os.clock()

for i = 0, 1000000-1, 1 do
	is_prime(i)
end
print(total, "\n");

print(os.clock()-start, "\n")
