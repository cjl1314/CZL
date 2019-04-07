<?php

class a {
	public $v1 = 1;
	public $v2 = 2;
	public $v3 = 3;
	public $v4 = 4;
	public $v5 = 5;
	public $v6 = 6;
}

class b {
	public $v1 = 1;
	public $v2 = 2;
	public $v3 = 3;
	public $v4 = 4;
	public $v5 = 5;
	public $v6 = 6;
	
	public $v7 = 7;
	public $v8 = 7;
	public $v9 = 7;
	public $v10 = 7;
	public $v11 = 7;
	public $v12 = 7;
}

$starttime = microtime(true);

$a = array();
for ($i = 0; $i < 1000*1000; $i++)
	$a[$i] = new a();
echo microtime(true)-$starttime."\n";
for ($i = 0; $i < 1000*1000; $i+=2)
	unset($a[$i]);
echo microtime(true)-$starttime."\n";
$b = array();
for ($i = 0; $i < 1000*1000; $i+=2)
	$b[$i] = new b();
	
echo microtime(true)-$starttime."\n";