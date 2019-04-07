import time
import math

total = 0
def isprime(num):
	global total
	count = int(math.sqrt(num))+1
	for i in xrange(2, count):
		if (num % i == 0):
			return 0
	total+=1
	return 1

start = time.time()
	
for i in xrange(0, 1000000):
	isprime(i)
print(total)

print(time.time()-start)
