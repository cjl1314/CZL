import time

start = time.time()

for i in xrange(0, 1000000):
	s = ""
	s += "foo"
	s += "bar"
	s += "ok"
	s += str(i)

print(time.time()-start)
print(s, "\n")
