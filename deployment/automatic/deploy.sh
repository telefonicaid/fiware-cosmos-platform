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
fab deploy_sftp

# Install MongoDB

# Start all services

# Install models

