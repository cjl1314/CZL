main
{
	int i, sum=0
	
	for i in (0, 10000000-1)
		if i <= 2500000
			sum += i
		else if i <= 5000000
			sum += i+1
		else if i <= 7500000
			sum += i+2
		else
			sum += i+3
		
	echo(sum)
	echo(clock())
}
