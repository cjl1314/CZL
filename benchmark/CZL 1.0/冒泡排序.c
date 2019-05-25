bubble_sort(&a) 
{
	size = #a
	for i in (0, size-1)
	{
		k = size-2-i
		for j in (0, k)
			if (a[j] < a[j+1])
				a[j] >< a[j+1]
	}
}

main
{
	var a
	for i in (0, 100000)
	{
		a = [0,1,2,3,4,5,6,7,8,9]
		bubble_sort(a)
	}
	echo(a)
	echo(clock())
}