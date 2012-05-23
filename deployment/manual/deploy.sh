#!/bin/bash
USAGE="Usage: `basename $0` [-c configuration] [-h]"
CONFIG="staging.cfg"
BASEDIR=`cd $(dirname $0); pwd`
PROJECT_ROOT=$BASEDIR/../..
INJECTION_SERVER_DIR=$PROJECT_ROOT/cosmos/platform/injection/server

# Parse script options
while getopts c:h option
do
    case $option in
        c)
            echo "[DEBUG] Changed configuration: $OPTARG"
            CONFIG=$OPTARG
            ;;
        h)
            echo $USAGE
            ;;
    esac
done

source $CONFIG

# Install and configure CDH

# Apply HUE patches and install Cosmos app

# Install SFTP server
cd ${INJECTION_SERVER_DIR}
#mvn package && \
scp -P 2222 target/injection-server-0.4.0.0-SNAPSHOT.jar \
    $FRONTEND_USER@$FRONTEND:~/injection
#&& \
#echo ssh -p 2222 $FRONTEND_USER:$FRONTEND_PASS@$FRONTEND nohup /usr/java/default/bin/java -jar injection-server-0.4.0.0-SNAPSHOT.jar --config ./injection_server.stage.properties &

# Install MongoDB

# Start all services

# Install models

