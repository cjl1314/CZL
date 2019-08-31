<?php

class a
{
	public $i, $sum;
	function a()
	{
		$t = microtime(true);
		$this->sum = 0;
		for ($this->i = 0; $this->i < 10000000; $this->i++)
			$this->sum+= $this->i;
		echo (microtime(true) - $t), "\n";
		echo $this->sum, "\n";
	}
}

$a = new a();
