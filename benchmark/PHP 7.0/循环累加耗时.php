<?php

$starttime = microtime(true);

$sum = 0;
$j = 0;
for ($i = 0; $i < 10000000; ++$i)
{
	$sum += $i + $j;
	$j++;
}
echo $sum, "\n";

echo microtime(true)-$starttime."\n";
