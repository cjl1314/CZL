<?php

$t = microtime(true);

for ($i = 0; $i < 1000000; ++$i)
{
	$s = "";
	$s .= "foo";
	$s .= "bar";
	$s .= "ok";
	$s .= $i;
}

echo (microtime(true) - $t), "\n";
echo $s, "\n";
