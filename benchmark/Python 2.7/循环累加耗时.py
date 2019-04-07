import time

start = time.time()

sum = 0
j = 0
for i in xrange(10000000):
	sum += i + j
	j += 1
print(sum, "\n")

print(time.time()-start)
