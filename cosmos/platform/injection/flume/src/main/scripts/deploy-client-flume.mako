connect ${client_master_host}:${client_master_port}

exec config ob-client 'tail("/tmp/cdrs")' 'agentE2ESink("${bridge_host}", ${bridge_port})'

