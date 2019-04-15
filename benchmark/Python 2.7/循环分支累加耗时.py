import time

start = time.time()

sum = 0
for i in xrange(10000000):
	if i <= 2500000:
		sum += i
	elif i <= 5000000:
		sum += i+1
	elif i <= 7500000:
		sum += i+2
	else:
		sum += i+3
print(sum, "\n")

print(time.time()-start)
