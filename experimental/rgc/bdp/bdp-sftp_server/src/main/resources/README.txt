###########################################################################################
# sftp_remote.sh 1.0                                                                      #
# Copyright 2012 PDI                                                                      #
# Author RubŽn Gonz‡lez Caballero rgc@tid.es                                              #
###########################################################################################


These instructions are for deploying and using the sftp server layer over HADOOP HDFS, and
a system to control the access serialization and compression of data into HDFS

Prerequisites
=============

	It is necessary bdp-utils-0.0.1-1.noarch.rpm and a installation of hadoop 0.20


Installations
=============

	rpm -i bdp-sftp_server-0.0.1-1.noarch.rpm


Usage
=====

	the script $INSTALLATION/run/bdp_sftp_server.sh  is used to control the creation 
	and termination of system-level processes.
	Start:
		> $INSTALLATION/run/bdp_sftp_server.sh start
		sftpserver started
		
	Stop:
		> $INSTALLATION/run/bdp_sftp_server.sh stop
		Shutting down sftpserver: ....
        sftpserver stopped
	
	Status:
		> $INSTALLATION/run/bdp_sftp_server.sh start
        Checking for sftpserver:   [RUNNING:STOPPED]
         
    The logs are stored in the path /var/log/bdp/sftpserver/*.log, the standard output 
    is redirected to /var/log/bdp/sftpserver/sftpserver.out, and the pid of the process
    is storage in /var/log/bdp/sftpserver/sftpserver.pid
    
    Any standard client could be connecting to this server. The probably unique 
    "strange thing" is configurate the port to connect to the server:
    > sftp -oPort=<PORT> <USER>@<SERVER>
    
    Note: the password of the user it is the username.

Configuration
=============

	The configuration is in the file $INSTALLATION/conf/sftp-server.properties
	
	sftp.port => port the sftpserver will listen on (default 8000)
	
	fs.default.name => The name of the HDFS file system
	
	filesystem.descriptor.class =>
	
		* es.tid.bdp.utils.BuilderDdpFileDescriptorXml 
			the filesystem configuration is stored into Mongodb
		* es.tid.bdp.utils.BuilderDdpFileDescriptorMongo
			The filesystem configurationi is stored into a XML
		* es.tid.bdp.utils.BuilderDdpFileDescriptorTextAllow
			All the user has access to all data, and load it raw.
	
	This block is necessary only if it is configure the MongoBuilder

	description.mongodb.host => the host in where is the mongod instance
    description.mongodb.port => Port the mongod will listen on (default 27017)
    description.mongodb.db => the database in mongodb
    description.mongodb.collection => the collection in mongodb


	This block is necessary only if it is configure the XmlBuilder
	
	descriptor.xml.file: path of xml file with the configuration



FileSystem controller
=====================

	The system controls the access, serialization and compression of data storage into
	the HDFS. The configuration is in tree. So if a user has one configuration in one
	path all the childrens have the same configuration unless they have their own
	configuration. the configuration has these fields
		*isReadable: if the user could read the file/directory.
		*isWritable: if the user could write the file o into the directory.
		*isCompressible: if the file is stored compressed in LZO format into the cluster
		*paser: if is null then the data is storage in raw data, Otherwise be needed 
		these field for serialize into a Protocol Buffer:
		 	className: the class who serialize the data.
		 	pattern: regular expression how describes the data.
		 	attr: the attribute of each group in the regular expression.

MongoDB fileSystem controller example
=====================================

db.filesystem.save(
{
    _id : "/",  
    users : {
        "hdfs" : {
             isReadble : true,
             isCompressible : false, 
             isWritable:true
         },
         "cdrs" : {
             isReadble : true,
             isCompressible : true, 
             isWritable:true,
             parser : {
                 className:"es.tid.bdp.utils.parse.ParserCdr", 
                 pattern : "(^.+)\\|(.*)\\|\\d\\|(\\d{2})/(\\d{2})/(\\d{4})\\|(\\d{2}):(\\d{2}):(\\d{2})\\|.*", 
                 attr : "userId|cellId|day|month|year|hour|minute|second"
             }
         },
         "weblog" : {
             isReadble : true,
             isCompressible : false, 
             isWritable:true,
             parser : {
                 className:"es.tid.bdp.utils.parse.ParserWebLog", 
                 pattern : "(^.+) (.*) (.*) (.*) (.*) (.*) (.*)", 
                 attr : "visitorId|fullUrl|date|httpStatus|mimeType|userAgent|method"
             }
         }
    }
})


Xml filesystem controller example
=================================

<?xml version="1.0" encoding="UTF-8"?>
<descriptions>
	<description path="/">
		<users>
			<user user="cdrs" isCompressible="true" isReadable="true" isWritable="true">
				<parser 
				    className="es.tid.bdp.utils.parse.ParserCdr"
					pattern="(^.+)\\|(.*)\\|\\d\\|(\\d{2})/(\\d{2})/(\\d{4})\\|(\\d{2}):(\\d{2}):(\\d{2})\\|.*"
					attr="userId|cellId|day|month|year|hour|minute|second" />
			</user>
			<user user="hdfs" isCompressible="false" isReadable="true" isWritable="true" />
			<user user="weblog" isCompressible="false" isReadable="true" isWritable="true">
				<parser 
				    className="es.tid.bdp.utils.parse.ParserWebLog"
					pattern="(^.+) (.*) (.*) (.*) (.*) (.*) (.*)"
					attr="visitorId|fullUrl|date|httpStatus|mimeType|userAgent|method" />
			</user>
		</users>
	</description>
</descriptions>


	I
	
	
