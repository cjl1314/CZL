<?php
ini_set('memory_limit', '2048M');

class a
{
	var $v1=1;
	var $v2=2;
	var $v3="foobar";
}

$a = array();
for ($i=0; $i<1000000; $i++)
	$a[$i] = new a;

$file='datas.txt';
if(false!==fopen($file,'w'))
  file_put_contents($file,serialize($a));

$handle=fopen($file,'r'); 
$a=unserialize(fread($handle,filesize($file)));
