class a
{
	var v1=1;
	var v2=2;
	var v3="foobar";
}

main
{
	f = open("datas.txt", "wb+")
	
	a = []
	for i in (0, 1000000-1)
		a[i] = new a

	write(f, a)
	echo(clock())

	read(f, a)
	echo(clock())
}
