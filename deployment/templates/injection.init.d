#!/bin/sh

# Provides:          Secret File Transfer Protocol server for Hadoop
#                    Distributed Filesystem
# Short-Description: Start and stop SFTP server for HDFS
# Description:       Start and stop Secret File Transfer Protocol server
#                    interfacing to a Hadoop Distributed Filesystem. Default
#                    sshd daemon must be stopped if the default port for SFTP
#                    (22) is to be respected.
#
# Author:            Luis Osa <logc@tid.es>
# Date:              23-05-2012

DESC="Injection Server"
NAME=`basename $0`
#SCRIPTDIR=`cd $(dirname $0); pwd`

PIDFILE="/var/run/injection/server.pid"
USAGE="Usage: ${NAME} [ start | stop | restart ]"

SERVERBIN="/usr/local/injection-server"
SERVERCFG="/etc/injection.cfg"
SERVERLOG="/var/log/injection/server.log"

if [ $# -lt 1 ]; then
    echo $USAGE
fi

function start () {
    if [ -f $PIDFILE ]; then
        RUNNING=$(ps aux | grep -c `cat ${PIDFILE}`)
        if [ $RUNNING != '0' ]; then
            echo "already running"
            exit 1
        fi
    fi
    # Needs to run as root to use port 22
    /usr/java/default/bin/java -jar $SERVERBIN --config $SERVERCFG &> $SERVERLOG &
    PID="$(ps -A -f | grep ${SERVERBIN} | awk '{ print $2 }' | head -1)"
    echo "started with process id ${PID}"
    echo $PID > $PIDFILE
}

function stop () {
    if [ -f $PIDFILE ]; then
        PID=`cat -- ${PIDFILE}`
        kill -s 9 $PID
        rm $PIDFILE
        echo "stopped as process id ${PID}"
    else
        echo "not running, or pid file not at ${PIDFILE}"
    fi
}

function restart () {
    stop
    sleep 1
    start
}

ACTION="$1"
case "$ACTION" in
    start)
        echo -n "Starting $DESC: $NAME "
        start
        echo "."
        ;;

    stop)
        echo -n "Stopping $DESC: $NAME "
        stop
        echo "."
        ;;

    restart)
        echo -n "Restarting $DESC: $NAME "
        restart
        echo "."
        ;;

    *)
        echo $USAGE
        exit 3
        ;;
esac

exit 0
