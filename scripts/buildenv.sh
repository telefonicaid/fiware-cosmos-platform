#
# Script to setup the build environment
#
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

SAMSON_HOME=$SAMSON_ROOT/home
SAMSON_WORKING=$SAMSON_ROOT/working
SAMSON_SS_LOG_DIR=$SAMSON_ROOT/logs
PATH=${SAMSON_HOME}/bin:${PATH}

mkdir -p $SAMSON_HOME $SAMSON_WORKING $SAMSON_SS_LOG_DIR

export SAMSON_HOME SAMSON_WORKING SAMSON_SS_LOG_DIR PATH

echo SAMSON_HOME is $SAMSON_HOME
echo SAMSON_WORKING is $SAMSON_WORKING
echo SAMSON_SS_LOG_DIR is $SAMSON_SS_LOG_DIR
