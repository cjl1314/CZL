var total = 0
int isprime(int n)
{
	count = int(os.sqrt(n))
	for i in (2, count)
		if (n%i == 0)
			return false
	$total++
	return true
}

main
{
	for i in (0, 1000000-1)
		isprime(i)
	
	echo($total)
	echo(clock())
}
