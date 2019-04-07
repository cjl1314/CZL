function sort(a)
	size = #a+1
	for i=1,size,1 do		
	  local k = size-1-i
      for j=1,k,1 do
		if a[j]<a[j+1] then
			a[j],a[j+1] = a[j+1],a[j]
		end
	  end		
	end
end

local start = os.clock()

local a
for i = 0, 100000-1, 1 do
	a={0,1,2,3,4,5,6,7,8,9}
	sort(a)
end
for i,vv in ipairs(a) do
	print(vv)
end

print(os.clock()-start, "\n")
