connect ${srv_master_host}:${srv_master_port}

exec config bridge 'collectorSource(${bridge_port})' console

