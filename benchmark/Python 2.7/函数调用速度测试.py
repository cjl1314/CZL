import time

def fib(i, j):
	return i+j

start = time.time()

sum = 0	
for i in xrange(0, 1000*10000):
	sum += fib(i, i+1)
print(sum)

print(time.time()-start)
