# This is the Ganglia Meta Daemon configuration file for Cosmos
#
#-------------------------------------------------------------------------------
# What to monitor. The most important section of this file.
# Format: 
# data_source "my cluster" [polling interval] address1:port addreses2:port ...
data_source "Cosmos" 10 127.0.0.1 ${monitored_hosts}

# Round-Robin Archives
# You can specify custom Round-Robin archives here (defaults are listed below)
#
# RRAs "RRA:AVERAGE:0.5:1:244" "RRA:AVERAGE:0.5:24:244" "RRA:AVERAGE:0.5:168:244" "RRA:AVERAGE:0.5:672:244" \
#      "RRA:AVERAGE:0.5:5760:374"
#

#
#-------------------------------------------------------------------------------
# Scalability mode. If on, we summarize over downstream grids, and respect
# authority tags. If off, we take on 2.5.0-era behavior: we do not wrap our output
# in <GRID></GRID> tags, we ignore all <GRID> tags we see, and always assume
# we are the "authority" on data source feeds. This approach does not scale to
# large groups of clusters, but is provided for backwards compatibility.
# default: on
# scalable off
#
#-------------------------------------------------------------------------------
# The name of this Grid. All the data sources above will be wrapped in a GRID
# tag with this name.
# default: Unspecified
gridname "Cosmos Cluster"

#-------------------------------------------------------------------------------
# The authority URL for this grid. Used by other gmetads to locate graphs
# for our data sources. Generally points to a ganglia/
# website on this machine.
# default: "http://hostname/ganglia/",
#   where hostname is the name of this machine, as defined by gethostname().
# authority "http://mycluster.org/newprefix/"
#
#-------------------------------------------------------------------------------
# List of machines this gmetad will share XML with. Localhost
# is always trusted.
# default: There is no default value
# trusted_hosts 127.0.0.1 169.229.50.165 my.gmetad.org
trusted_hosts 127.0.0.1
#
#-------------------------------------------------------------------------------
# If you want any host which connects to the gmetad XML to receive
# data, then set this value to "on"
# default: off
# all_trusted on
#
#-------------------------------------------------------------------------------
# If you don't want gmetad to setuid then set this to off
# default: on
# setuid off
#
#-------------------------------------------------------------------------------
# User gmetad will setuid to (defaults to "ganglia")
# default: "ganglia"
# setuid_username "ganglia"
#
#-------------------------------------------------------------------------------
# The port gmetad will answer requests for XML
# default: 8651
# xml_port 8651
#
#-------------------------------------------------------------------------------
# The port gmetad will answer queries for XML. This facility allows
# simple subtree and summation views of the XML tree.
# default: 8652
# interactive_port 8652
#
#-------------------------------------------------------------------------------
# The number of threads answering XML requests
# default: 4
# server_threads 10
#
#-------------------------------------------------------------------------------
# Where gmetad stores its round-robin databases
# default: "/var/lib/ganglia/rrds"
# rrd_rootdir "/some/other/place"
