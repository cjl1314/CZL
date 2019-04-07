<?php

function fib(int $i, int $j):int
{
	return $i+$j;
}

$starttime = microtime(true);

$sum = 0;
for ($i = 0; $i < 1000*10000; $i++)
	$sum += fib($i, $i+1);

echo $sum, "\n";

echo microtime(true)-$starttime."\n";