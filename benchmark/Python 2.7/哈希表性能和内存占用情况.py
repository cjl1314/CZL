import time

class a:
	#__slots__ = ('a', 'b')
	def plus(self):
		self.v1 = 1
		self.v2 = 1
		self.v3 = 1
		self.v4 = 1
		self.v5 = 1
		self.v6 = 1
		
class b:
	#__slots__ = ('a', 'b')
	def plus(self):
		self.v1 = 1
		self.v2 = 1
		self.v3 = 1
		self.v4 = 1
		self.v5 = 1
		self.v6 = 1
		self.v7 = 1
		self.v8 = 1
		self.v9 = 1
		self.v10 = 1
		self.v11 = 1
		self.v12 = 1

start = time.time()
		
t1 = {}
for i in xrange(0, 1000*1000):
	t1[i] = a()
end = time.time()
print(end-start)
for i in xrange(1, 1000*1000, 2):
	t1.pop(i)
end = time.time()
print(end-start)
t2 = {}
for i in xrange(1, 1000*1000, 2):
	t2[i] = b()
end = time.time()

print(time.time()-start)