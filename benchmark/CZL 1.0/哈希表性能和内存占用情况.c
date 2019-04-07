class a
{
	var v1=1, v2=2, v3=3, v4=4, v5=5, v6=6
}

class b
{
	var v1=1, v2=2, v3=3, v4=4, v5=5, v6=6
	var v7=1, v8=2, v9=3, v10=4, v11=5, v12=6
}

main
{
	setrank(0)
	
	a = {}
	for i in (0, 1000*1000)
		a[i] = new a
	echo(clock())
	
	for i in (0, 1000*1000, 2)
		i => a
	echo(clock())
	
	b = {}
	for i in (0, 1000*1000, 2)
		b[i] = new b
	echo(clock())
}
