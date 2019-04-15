<?php

$t = microtime(true);

$sum = 0;
for ($i = 0; $i < 10000000; ++$i)
{
	if ($i <= 2500000)
		$sum += $i;
	else if ($i <= 5000000)
		$sum += $i+1;
	else if ($i <= 7500000)
		$sum += $i+2;
	else
		$sum += $i+3;
}
echo $sum, "\n";

print (microtime(true) - $t);
