Tektronix Tools
===============

Sniffer
-------

    ./sniffer [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)] [-host (hostname)] [-port (port to connect to)] [-filePrefix (prefix)] [-timeFormat (time format for storage file)]

Note that you can specify the name of the file and when the file is
rotated/rolled  over . The "-timeFormat" option  controls 2 things, the date in
the file name and when the sniffer closes the dump file and starts a new one.
The values provided are based format charactersfor the C function strftime(3C).

For example to get the dump files to rotate/rollover every day you could
use "%Y-%m-%d" (the "-" are separation characters that you supply,
if you don't want these characters then you could equally do "%Y%m%d"). By
default if "-timeFormat" is not specified "%Y-%m-%d" is used. If
you want it to roll over every hour use, "%Y-%m-%d %H". 

The flag "-filePrefix" defines the name of the file (without the timestamp) and
efaults to "tektronixPackets" if not set on the command line. The path the files
get written to is hard-coded to /data/indigo and cannot be changed (without
recompiling the source code).

Tunnel 
------

    ./tektronixTunnel [-tektronix (ip:port)] [-samson (ip:port)] [-sniffer (ip:port)] [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)] [-filter]

Setup a tunnel/bridge between tektronix and samson with the option to hook a
sniffer that can dump the data stream to flat files.

Arcanum Simulator
-----------------

    ./tektronix [-host (host)] [-port (port)] [-sleepTime (microsecs)] [-sleepEach (loops)] [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)]

This is the new simulator program. Every "-sleep" microseconds it sends a 400
byte packet to the designated host/port. Testing on a Solaris VM we see a limit
of about 100 packets per second (-sleep 10000) being logged in the sniffer.
This appears to be an OS restriction as the OS checks the clock every 10ms
(http://docs.oracle.com/cd/E19683-01/806-7009/chapter2-83/index.html and
http://blogs.oracle.com/jtc/entry/overhead_in_increasing_the_solaris). In
theory this will only affect the programs that use usleep(). Changing
hires_tick and hires_hz in /etc/system will allow us to generate a higher rate
of packets per second, although I've not tested it.

Testing the 3 programs
----------------------

If you run the following programs, each in their own terminal/screen session,
with a spare to look at the dumps from the sniffer, you can get a feel for how
the tools work without having to connect to the actual Arcanum stream.

tektronixTunnel -tektronix localhost:2001 -sniffer localhost:2002 -samson localhost:2003 -v 
sniffer -host localhost -port 2002 -vvv -timeFormat "%Y%m%d%H%M" # rotate the files every minute
tektronix -host localhost -port 2001 -sleep 1000 -v  # try to generate 1000 packets (400 bytes each) every second.

In the fourth session/window check the files in /data/indigo to see they are
being created and that every minute they are rolling over into a new file. You
should see the file size of  2400000 bytes +/- 800 bytes
	

