<?php

function fib($i)
{
	if ($i > 0) fib($i-1);
	sleep(100000);
}

fib(1000000);
