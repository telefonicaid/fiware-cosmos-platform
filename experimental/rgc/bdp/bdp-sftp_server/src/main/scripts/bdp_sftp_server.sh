#!/bin/bash

baseDir=/usr/local/bdp/sftpserver                                # Base dir of installation
javaCommand="java"                                               # name of the Java launcher without the path
javaExe="$JAVA_HOME/bin/$javaCommand"                            # file name of the Java application launcher executable
LIB_DIR=$baseDir/lib                                             # project library jars directory
PLUGINS=$baseDir/plugins                                         # plugins library jars directory
UTIL_LIB=/usr/local/bdp/utils/lib                                # utils library jars directory
HADOOP_LIB=$HADOOP_HOME/lib 	                                 # Hadoop library jars directory

CONF=$baseDir/conf                                               # configuration directory

pidFile=/var/log/bdp/sftpserver/sftpserver.pid                   # pid file of sftpserver process when running
outFile=/var/log/bdp/sftpserver/sftpserver.out                   # pid file of sftpserver process when running

 

DEBUG_OPTIONS="-Xdebug -Xrunjdwp:transport=dt_socket,address=8011,server=y,suspend=y"
#JAVA_OPTS="$JAVA_OPTS $DEBUG_OPTIONS"

JAVA_OPTS="$JAVA_OPTS -Djava.library.path=$UTIL_LIB/native:/opt/hadoop/lib/native/Linux-amd64-64"
JAVA_OPTS="$JAVA_OPTS -Dlog4j.configuration=file://$CONF/log4j.xml"
JAVA_OPTS="$JAVA_OPTS -Dproperties.configuration=/$CONF/sftp-server.properties"



# Include all dependencies in lib folder and append to classpath
for LIB in $LIB_DIR/*.jar; do
   CLASSPATH=$CLASSPATH:$LIB
done

# Include all dependencies in utils/lib folder and append to classpath
for LIB in $UTIL_LIB/*.jar; do
   CLASSPATH=$CLASSPATH:$LIB
done

# Include all dependencies in hadoop lib folder and append to classpath
for LIB in $HADOOP_LIB/*.jar; do
   CLASSPATH=$CLASSPATH:$LIB
done


# Include Hadoop core jar
CLASSPATH=$HADOOP_HOME/hadoop-core.jar:$CLASSPATH

javaArgs=" -cp $CLASSPATH  es.tid.bdp.sftp.server.HdfsSftpServer"     # arguments for Java launcher
javaCommandLine="$javaExe $JAVA_OPTS $javaArgs"           		      # JVM command line 


serviceName="sftpserver"                                              # service name
maxShutdownTime=15                                                    # maximum number of seconds to wait for the daemon to terminate normally




# Returns 0 if the process with PID $1 is running.
function checkProcessIsRunning {
    local pid="$1"
    if [ -z "$pid" -o "$pid" == " " ]; then return 1; fi
    if [ ! -e /proc/$pid ]; then return 1; fi
    return 0; 
}

# Returns 0 if the process with PID $1 is our Java service process.
function checkProcessIsOurService {
    local pid="$1"
    if [ "$(ps -p $pid --no-headers -o comm)" != "$javaCommand" ]; then return 1; fi
    grep -q --binary -F "$javaCommandLineKeyword" /proc/$pid/cmdline
    if [ $? -ne 0 ]; then return 1; fi
    return 0; 
}

# Returns 0 when the service is running and sets the variable $pid to the PID.
function getServicePID {
    if [ ! -f $pidFile ]; then return 1; fi
    pid=$(head -1 $pidFile)
    checkProcessIsRunning $pid || return 1
    checkProcessIsOurService $pid || return 1
    return 0; 
}

start() {
    getServicePID
    if [ $? -eq 0 ]; then echo "$serviceName is already running"; exit 2; fi
    nohup $javaCommandLine &> $outFile  &
    echo $! >$pidFile
    sleep 0.5

    pid=$(head -1 $pidFile)
    if checkProcessIsRunning $pid; then 
        echo $serviceName started
        return 0   
    else
        echo "$serviceName start failed, see logfile."
        return 1 
    fi
}

stop() {
    getServicePID
    if [ $? -ne 0 ]; then echo "$serviceName is not running"; return 0; fi
    echo -n "Shutting down $serviceName: "
    kill $pid || return 1
    for ((i=0; i<maxShutdownTime*10; i++)); do
        checkProcessIsRunning $pid
        if [ $? -ne 0 ]; then
            rm -f $pidFile
            echo -e "\n $serviceName stopped"
            return 0
        fi
        echo -n "."
        sleep 0.1
    done
    kill -9 $pid || return 1
    for ((i=0; i<maxShutdownTime*10; i++)); do
        checkProcessIsRunning $pid
        if [ $? -ne 0 ]; then
            rm -f $pidFile
            echo "$serviceName stopped"
            return 0
        fi
        sleep 0.1
    done
    echo  "$serviceName did not terminate within $maxShutdownTime seconds, sending SIGKILL..."
    return 1
}

status() {
       echo -n "Checking for $serviceName:   "
      
      if getServicePID; then
           echo "RUNNING";
           return 0;
      else
           echo "STOPPED";
           return 3;
      fi
}

case "$1" in
    start)
        start
        ;;
    stop)
        stop
        ;;
    status)
        status
        ;;
    restart)
        stop
        start
        ;;
    *)
        echo "Usage:  {start|stop|status|restart}"
        exit 1
        ;;
esac
exit $?
