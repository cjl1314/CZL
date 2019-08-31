import time

start = time.time()

a = "foo"
b = "bar"
c = "ok"
for i in xrange(0, 1000000):
	s = a+b+c+str(i)

print(time.time()-start)
print(s, "\n")
