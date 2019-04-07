<?php

$total = 0;
function isprime(int $num): int {
	global $total;
	$count = (int) sqrt($num) + 1;
	for ($i = 2; $i < $count; ++$i) {
		if ($num%$i==0)
			return false;
	}
	$total++;
	return true;
}

$starttime = microtime(true);

for ($i = 0; $i < 1000000; $i++)
	isprime($i);
echo $total, "\n";

echo microtime(true)-$starttime."\n";