import time

class a:
	def a(self):
		start = time.time()
		self.sum = 0
		for self.i in xrange(0, 10000000):
			self.sum += self.i
		print(time.time()-start)
		print(self.sum, "\n")

a = a()
a.a()
