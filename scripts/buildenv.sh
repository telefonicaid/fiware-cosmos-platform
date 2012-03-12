#
# Script to setup the build environment
#

# Base directory for installation
if [ -z $1 ];
then
    SAMSON_ROOT=$HOME/.samson
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
SAMSON_SS_LOG_DIR=$SAMSON_ROOT/logs

PATH=${SAMSON_HOME}/bin:${PATH}

OS=$(uname -s)

# Setup the Shared library path
if [ ${OS} = "Linux" ]; then
    LD_LIBRARY_PATH=${SAMSON_HOME}/lib:${LD_LIBRARY_PATH}
elif [ ${OS} = "Darwin" ]; then
    DYLD_LIBRARY_PATH=${SAMSON_HOME}/lib:${DYLD_LIBRARY_PATH}
fi

# Whoami?
SAMSON_OWNER=$(whoami)

mkdir -p $SAMSON_HOME $SAMSON_WORKING $SAMSON_SS_LOG_DIR

export SAMSON_HOME SAMSON_WORKING SAMSON_SS_LOG_DIR PATH SAMSON_OWNER

echo SAMSON_HOME is $SAMSON_HOME
echo SAMSON_WORKING is $SAMSON_WORKING
echo SAMSON_SS_LOG_DIR is $SAMSON_SS_LOG_DIR
