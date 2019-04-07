main
{
	sum = 0
	j = 0
	for i in (0, 10000000-1)
	{
		sum += i+j
		j++
	}
	echo(sum)
	echo(clock())
}
