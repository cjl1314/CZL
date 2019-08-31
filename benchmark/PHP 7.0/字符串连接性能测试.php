<?php

$t = microtime(true);

$s = "";
$a = "foo";
$b = "bar";
$c = "ok";
for ($i = 0; $i < 1000000; ++$i)
	$s = $a.$b.$c.$i;

echo (microtime(true) - $t), "\n";
echo $s, "\n";
