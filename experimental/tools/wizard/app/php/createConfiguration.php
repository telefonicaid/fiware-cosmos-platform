<?php
    $con = mysql_connect('localhost', 'root', '');
	if (!$con)
	{
		die('Could not connect: ' . mysql_error());
	}
	
	mysql_select_db("BDP", $con);
	
//	$sql = "INSERT INTO wizardDB (name, inputType, inputPath, aggregations,outputType,state) VALUES (' $_POST[name]',' $_POST[inputType]','$_POST[inputPath]','$_POST[aggregations]','$_POST[outputType]','$_POST[state]' ); COMMIT;";
	
	$sql = "INSERT INTO wizardDB (name,inputType, inputPath, aggregations,outputType) VALUES ('test1','File','pathRoute','Views per Protocol','Data Explotation' );";
	
	if (!mysql_query($sql))
  	{
  		die('Error: ' . mysql_error());
  	}
	
	mysql_close($con);		
?>