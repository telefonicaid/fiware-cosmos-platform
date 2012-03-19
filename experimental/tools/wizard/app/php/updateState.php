<?php
    $con = mysql_connect('localhost', 'root', '');
	if (!$con)
	{
		die('Could not connect: ' . mysql_error());
	}
	
	mysql_select_db("BDP", $con);
	
	$sql = "UPDATE wizardDB set state=$_POST[state]  WHERE id=$_POST[id]; COMMIT;";
	
	if (!mysql_query($sql))
  	{
  		die('Error: ' . mysql_error());
  	}
	
	mysql_close($con);	
?>