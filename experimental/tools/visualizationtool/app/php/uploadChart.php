<?php

	$con = mysql_connect('localhost', 'root', '');
	if (!$con)
	{
		die('Could not connect: ' . mysql_error());
	}
	
	mysql_select_db("chartsDB", $con);
	
	$sql = "INSERT INTO charts (title, type, xAxis,yAxis,series) VALUES (' $_POST[title]','$_POST[type]','$_POST[xAxisTitle]','$_POST[yAxisTitle]','$_POST[dataset]' );";
	
	if (!mysql_query($sql))
  	{
  		die('Error: ' . mysql_error());
  	}
	
	mysql_close($con);		
?>

