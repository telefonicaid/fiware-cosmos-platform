#
# Script to setup the build environment
#

SAMSON_VERSION=0.6.1
SAMSON_TEST_HOST=localhost
SAMSON_WORKER_PORT_ENV=1326
SAMSON_WORKER_WEB_PORT_ENV=1204
SAMSON_LOGQUERY_PORT_ENV=6002
SAMSON_LOG_PORT_ENV=6003

SAMSON_LOG_LEVELS="-t 15,20,22,31,32,33,34,35,210"

# Base directory for installation
if [ -z $1 ];
then
    SAMSON_ROOT=$HOME/.samson-$SAMSON_VERSION
else
    SAMSON_ROOT=$1
fi

echo "Using $SAMSON_ROOT as the base directory"

if [ ! -d $SAMSON_ROOT ];
then
    mkdir -p $SAMSON_ROOT
fi

# Setup variables based on SAMSON_ROOT
SAMSON_HOME=$SAMSON_ROOT/home
SAMSON_WORKING=$SAMSON_ROOT/working
#samsonPush/Pop... log directory
LOG_DIR=$SAMSON_ROOT/logs
#samsonWorker log directory
SS_WORKER_LOG_DIR=$LOG_DIR
#delilah log directory
SS_DELILAH_LOG_DIR=$LOG_DIR
#logServer log directory
LOGSERVER_LOG_DIR=$SAMSON_ROOT/logserver

PATH=${SAMSON_HOME}/bin:${PWD}/scripts:${PATH}

OS=$(uname -s)

# Setup the Shared library path
if [ ${OS} = "Linux" ]; then
    LD_LIBRARY_PATH=${SAMSON_HOME}/lib:${LD_LIBRARY_PATH}
elif [ ${OS} = "Darwin" ]; then
    DYLD_LIBRARY_PATH=${SAMSON_HOME}/lib:${DYLD_LIBRARY_PATH}
fi

# Whoami?
SAMSON_OWNER=$(whoami)

mkdir -p $SAMSON_HOME $SAMSON_WORKING $SS_WORKER_LOG_DIR $LOG_DIR

export SAMSON_HOME SAMSON_WORKING LOG_DIR SS_DELILAH_LOG_DIR SS_WORKER_LOG_DIR PATH SAMSON_OWNER SAMSON_VERSION SAMSON_WORKER_PORT_ENV SAMSON_WORKER_WEB_PORT_ENV SAMSON_LOG_PORT_ENV LOGSERVER_LOG_DIR SAMSON_LOGQUERY_PORT_ENV SAMSON_LOG_LEVELS SAMSON_TEST_HOST 

echo SAMSON_HOME is $SAMSON_HOME
echo SAMSON_WORKING is $SAMSON_WORKING
echo SS_WORKER_LOG_DIR is $SS_WORKER_LOG_DIR
echo LOG_DIR is $LOG_DIR

alias sgrep="egrep --exclude=*.svn*"

