import time

def bubble_sort(lst):
	length = len(lst)
	for i in xrange(0, length):
		k = length-1-i
		for j in xrange(0, k):
			if lst[j] < lst[j+1]:
				lst[j], lst[j+1] = lst[j+1], lst[j]

start = time.time()

for c in xrange(100000):
	lst = [0,1,2,3,4,5,6,7,8,9]
	bubble_sort(lst)
print(lst)

print(time.time()-start)
