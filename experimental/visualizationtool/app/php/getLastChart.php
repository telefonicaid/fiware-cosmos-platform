<?php
	$con = mysql_connect('localhost', 'root', '');
	if (!$con)
	{
		die('Could not connect: ' . mysql_error());
	}
	
	mysql_select_db("chartsDB", $con);
	
	$sql = "SELECT * FROM charts ORDER BY id DESC LIMIT 1;";
	$result = mysql_query($sql);
	
	if (!$result)
  	{
  		die('Error: ' . mysql_error());
  	}
	
    $row = mysql_fetch_row($result);
	echo json_encode(array ("row" => $row));
	// echo $row[0];
	// echo $row[1];
	// echo $row[2];
	// echo $row[3];
	// echo $row[4];
	// echo $row[5];
	mysql_close($con);
?>