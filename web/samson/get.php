<?php

function printCommand( $command )
{ 
	$host = "localhost";
	$puerto = 9898;

	$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	if (socket_connect($socket, $host, $puerto))
   	{

 		$salida = ' ';
 		socket_write($socket, $command);
		while ($salida = socket_read($socket, 2048)){
			echo $salida;

		}
	   socket_close($socket);			
   	}   	
} 

printCommand(  "get -quit\n" );

?>

