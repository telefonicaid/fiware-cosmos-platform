<?php
	$q=$_GET["q"];
	
	$con = mysql_connect('localhost', 'root', 'tool');
	if (!$con)
	{
		die('Could not connect: ' . mysql_error());
	}
	
	mysql_select_db("chartsDB", $con);
	
	$sql="SELECT * FROM charts WHERE id = '".$q."'";
	
	$result = mysql_query($sql);
	$output = mysql_fetch_array($result);
	
	mysql_close($con);
?>