#!/bin/bash
# infinity-server : This starts and stops infinity-server daemon
#
# chkconfig: 2345 20 80
# description: Cosmos Infinity Storage filesystem server
# processname: infinity-server
# pidfile: /var/run/infinity-server.pid

# Source function library.

. /etc/rc.d/init.d/functions

cosmosdir="/opt/pdi-cosmos/infinity-server"
pname="infinity-server"
config="config.file=${cosmosdir}/infinity-server.conf"
logging="logback.configurationFile=${cosmosdir}/logback.xml"
exe="java -D$config -D$logging -jar ${cosmosdir}/infinity-server.jar"
pidfile="/var/run/${pname}.pid"
user="root"

RETVAL=0

start() {
    echo -n "Starting $pname : "
    if [ -s ${pidfile} ]; then
       RETVAL=1
       echo -n "Already running !" && warning
    else
        touch $pidfile
        chown $user $pidfile
        su -s /bin/sh $USER -c "
                cd /
                exec setsid ${exe}   \
                </dev/null >/dev/null 2>&1 &
                echo \$! >${pidfile}
                disown \$!
                "
        PID=`cat $pidfile`
        [ $PID ] && success || failure
    fi
    echo
}

stop() {
    echo -n "Shutting down $pname : "
    if [ -f $pidfile ]; then
        PID=`cat $pidfile`
        kill -s TERM $PID
        RETVAL=$?
        [ $RETVAL -eq 0 ] && success || failure
        rm -f $pidfile
    else
        echo -n "Not running" && failure
    fi
    echo
}

restart() {
    echo -n "Restarting $pname : "
    stop
    sleep 5
    start
}

case "$1" in
    start)
        start
    ;;
    stop)
        stop
    ;;
    status)
        status ${pname}
    ;;
    restart)
        restart
    ;;
    *)
        echo "Usage: $0 {start|stop|status|restart}"
    ;; esac
