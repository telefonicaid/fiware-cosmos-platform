#!/bin/bash
# Source function library.
. /etc/init.d/functions

# Source profile
. /etc/environment

baseDir=/usr/local/bdp/sftpclient                                # Base dir of installation
javaCommand="java"                                               # name of the Java launcher without the path
javaExe="$JAVA_HOME/bin/$javaCommand"                            # file name of the Java application launcher executable
LIB_DIR=$baseDir/lib                                                 # project library jars directory
PLUGINS=$baseDir/plugins                                         # plugins library jars directory
UTIL_LIB=/usr/local/bdp/utils/lib                                # utils library jars directory
HADOOP_LIB=$HADOOP_HOME/lib 	                                 # Hadoop library jars directory

CONF=$baseDir/conf                                              # configuration directory
 


JAVA_OPTS="$JAVA_OPTS -Djava.library.path=/usr/lib64:/opt/hadoop/lib/native/Linux-amd64-64"
JAVA_OPTS="$JAVA_OPTS -Dlog4j.configuration=file://$CONF/log4j.xml"

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

# Include all dependencies in hadoop plugins and append to classpath
for LIB in $PLUGINS/*.jar; do
   CLASSPATH=$CLASSPATH:$LIB
done

# Include Hadoop core jar
CLASSPATH=$HADOOP_HOME/hadoop-core.jar:$CLASSPATH

javaArgs=" -cp $CLASSPATH  es.tid.bdp.sftp.client.MainSftpClient"     # arguments for Java launcher
javaCommandLine="$javaExe $JAVA_OPTS $javaArgs $@"           		  # JVM command line 


echo "$javaCommandLine"
$javaCommandLine



