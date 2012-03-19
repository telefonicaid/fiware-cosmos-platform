<?php

	# Connection data to the data base
	$server = 'localhost';
	$user   = 'root';
	$password  = '';
	$database = 'BDP';
	 
	# Establish connection to the data base
	$connection = mysql_connect($server, $user, $password);
	mysql_select_db($database);
	 
	# Data base table = "charts" / primary key = "id"
	$q = "SELECT * FROM wizardDB ORDER BY id DESC";
	$rs = mysql_query($q);
	
	$sqlArray=array();
	
	while ($row = mysql_fetch_array($rs, MYSQL_ASSOC))
	{
	$sqlArray[]=$row;
	}
	
	echo json_encode($sqlArray);
	
	mysql_free_result($rs);
	
	#Close connection with the data base
	mysql_close($connection);

?>