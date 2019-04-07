<?php

function bubble_sort(&$array) {
	$size = count($array);
	for ($i=0;$i<$size;$i++) {
		$k = $size-1-$i;
		for ($j=0;$j<$k;$j++) {
			if ($array[$j] < $array[$j+1]) {
				$temp = $array[$j];
				$array[$j] = $array[$j+1];
				$array[$j+1] = $temp;
			}
		}
	}
}

$starttime = microtime(true);

for ($c=0;$c<100000;$c++)
{
	$array = array(0,1,2,3,4,5,6,7,8,9);
	bubble_sort($array);
}
print_r($array);

echo microtime(true)-$starttime."\n";