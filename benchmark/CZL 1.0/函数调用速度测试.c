int fib(int i, int j)
{
	return i+j
}

main()
{
	sum := 0
	for i in (0, 1000*10000-1)
		sum += fib(i, i+1)
	echo(sum)
	echo(clock())
}
