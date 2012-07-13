globals {                    
  daemonize = yes              
  setuid = yes             
  user = ganglia              
  debug_level = 0               
  max_udp_msg_len = 1472        
  mute = no             
  deaf = no             
  host_dmax = 0 /*secs */ 
  cleanup_threshold = 300 /*secs */ 
  gexec = no             
} 

cluster {
  name = "Cosmos"
  owner = "Cosmos"
}

host {
  location = "MAD"
}

udp_send_channel {
  #bind_hostname = yes # Highly recommended, soon to be default.
                       # This option tells gmond to use a source address
                       # that resolves to the machine's hostname.  Without
                       # this, the metrics may appear to come from any
                       # interface and the DNS names associated with
                       # those IPs will be used to create the RRDs.
  host = ${gmetad_host}
  port = 8649
}

udp_recv_channel {
  port = 8649
}

tcp_accept_channel {
  port = 8649
}

/* The old internal 2.5.x metric array has been replaced by the following 
   collection_group directives.  What follows is the default behavior for 
   collecting and sending metrics that is as close to 2.5.x behavior as 
   possible. */

/* This collection group will cause a heartbeat (or beacon) to be sent every 
   20 seconds.  In the heartbeat is the GMOND_STARTED data which expresses 
   the age of the running gmond. */ 
collection_group { 
  collect_once = yes 
  time_threshold = 20 
  metric { 
    name = "heartbeat" 
  } 
} 

/* This collection group will send general info about this host every 1200 secs. 
   This information doesn't change between reboots and is only collected once. */ 
collection_group { 
  collect_once = yes 
  time_threshold = 1200 
  metric { 
    name = "cpu_num" 
  } 
  metric { 
    name = "cpu_speed" 
  } 
  metric { 
    name = "mem_total" 
  } 
  /* Should this be here? Swap can be added/removed between reboots. */ 
  metric { 
    name = "swap_total" 
  } 
  metric { 
    name = "boottime" 
  } 
  metric { 
    name = "machine_type" 
  } 
  metric { 
    name = "os_name" 
  } 
  metric { 
    name = "os_release" 
  } 
  metric { 
    name = "location" 
  } 
} 

/* This collection group will send the status of gexecd for this host every 300 secs */
/* Unlike 2.5.x the default behavior is to report gexecd OFF.  */ 
collection_group { 
  collect_once = yes 
  time_threshold = 300 
  metric { 
    name = "gexec" 
  } 
} 

/* This collection group will collect the CPU status info every 20 secs. 
   The time threshold is set to 90 seconds.  In honesty, this time_threshold could be 
   set significantly higher to reduce unneccessary network chatter. */ 
collection_group { 
  collect_every = 20 
  time_threshold = 90 
  /* CPU status */ 
  metric { 
    name = "cpu_user"  
    value_threshold = "1.0" 
  } 
  metric { 
    name = "cpu_system"   
    value_threshold = "1.0" 
  } 
  metric { 
    name = "cpu_idle"  
    value_threshold = "5.0" 
  } 
  metric { 
    name = "cpu_nice"  
    value_threshold = "1.0" 
  } 
  metric { 
    name = "cpu_aidle" 
    value_threshold = "5.0" 
  } 
  metric { 
    name = "cpu_wio" 
    value_threshold = "1.0" 
  } 
  /* The next two metrics are optional if you want more detail... 
     ... since they are accounted for in cpu_system.  
  metric { 
    name = "cpu_intr" 
    value_threshold = "1.0" 
  } 
  metric { 
    name = "cpu_sintr" 
    value_threshold = "1.0" 
  } 
  */ 
} 

collection_group { 
  collect_every = 20 
  time_threshold = 90 
  /* Load Averages */ 
  metric { 
    name = "load_one" 
    value_threshold = "1.0" 
  } 
  metric { 
    name = "load_five" 
    value_threshold = "1.0" 
  } 
  metric { 
    name = "load_fifteen" 
    value_threshold = "1.0" 
  }
} 

/* This group collects the number of running and total processes */ 
collection_group { 
  collect_every = 80 
  time_threshold = 950 
  metric { 
    name = "proc_run" 
    value_threshold = "1.0" 
  } 
  metric { 
    name = "proc_total" 
    value_threshold = "1.0" 
  } 
}

/* This collection group grabs the volatile memory metrics every 40 secs and 
   sends them at least every 180 secs.  This time_threshold can be increased 
   significantly to reduce unneeded network traffic. */ 
collection_group { 
  collect_every = 40 
  time_threshold = 180 
  metric { 
    name = "mem_free" 
    value_threshold = "1024.0" 
  } 
  metric { 
    name = "mem_shared" 
    value_threshold = "1024.0" 
  } 
  metric { 
    name = "mem_buffers" 
    value_threshold = "1024.0" 
  } 
  metric { 
    name = "mem_cached" 
    value_threshold = "1024.0" 
  } 
  metric { 
    name = "swap_free" 
    value_threshold = "1024.0" 
  } 
} 

collection_group { 
  collect_every = 40 
  time_threshold = 300 
  metric { 
    name = "bytes_out" 
    value_threshold = 4096 
  } 
  metric { 
    name = "bytes_in" 
    value_threshold = 4096 
  } 
  metric { 
    name = "pkts_in" 
    value_threshold = 256 
  } 
  metric { 
    name = "pkts_out" 
    value_threshold = 256 
  } 
}

/* Different than 2.5.x default since the old config made no sense */ 
collection_group { 
  collect_every = 1800 
  time_threshold = 3600 
  metric { 
    name = "disk_total" 
    value_threshold = 1.0 
  } 
}

collection_group { 
  collect_every = 40 
  time_threshold = 180 
  metric { 
    name = "disk_free" 
    value_threshold = 1.0 
  } 
  metric { 
    name = "part_max_used" 
    value_threshold = 1.0 
  } 
}

