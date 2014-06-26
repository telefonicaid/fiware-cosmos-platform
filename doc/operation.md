# Cosmos operation
This documents describes how to operate a Cosmos deployment.

## Command line administration (<code>cosmos-admin</code>)

The <code>cosmos-admin</code> tool is installed in <code>/opt/pdi-cosmos/cosmos-admin/</code> within a Cosmos deployment. It allows an admin user to setup the platforn and to manage the persistent storage (setup and termination), the termination of clusters, the user profiles and the user groups. The subcommands below can be obtained by typing:

    $ ./cosmos-admin --help

### Setup the platform
General subcommand:

    $ ./cosmos-admin setup
          --help   Show help message

### Persistent storage management
General subcommand:

    $ ./cosmos-admin persistent-storage
          --help   Show help message

Setup the persistent storage:

    $ ./cosmos-admin persistent-storage setup
          --help   Show help message

Terminate the persistent storage:

    $ ./cosmos-admin persistent-storage terminate
          --help   Show help message

### Clusters managment
General subcommand:

    $ ./cosmos-admin cluster
          --help   Show help message

Terminate a certain cluster, given the clusterid:

    $ ./cosmos-admin cluster terminate
          -c, --clusterid  <arg>
          --help               Show help message

### Profiles management
General subcommand:

    $ ./cosmos-admin profile
          --help   Show help message

Set the quota limit for certain profile, identified by handle:

    $ ./cosmos-admin profile set-machine-quota
          -h, --handle  <arg>
          -l, --limit  <arg>
          --help            Show help message

Remove the quota from certain profile, identified by handle:

    $ ./cosmos-admin profile remove-machine-quota
          -h, --handle  <arg>
          --help            Show help message

Enable certain capability to certain profile, identified by handle:

    $ ./cosmos-admin profile enable-capability
          -c, --capability  <arg>
          -h, --handle  <arg>
          --help                Show help message

Disable certain capability to certain profile, identified by handle:

    $ ./cosmos-admin profile disable-capability
          -c, --capability  <arg>
          -h, --handle  <arg>
	  --help                Show help message

Set a group for the profile, identified by handle:

    $ ./cosmos-admin profile set-group
          -g, --group  <arg>
          -h, --handle  <arg>
          --help            Show help message

Remove a profile, identified by handle, from its group, if any:

    $ ./cosmos-admin profile remove-group
          -h, --handle  <arg>
          --help            Show help message

List all the profiles:

    $ ./cosmos-admin profile list
          --help   Show help message

### Groups management
General subcommand:

    $ ./cosmos-admin group
          --help   Show help message

Create a group, given its name and minimum quota, if any:

    $ ./cosmos-admin group create
          -m, --min-quota  <arg>    (default = 0)
          -n, --name  <arg>
          --help               Show help message

List all the groups:

    $ ./cosmos-admin group list
          --help   Show help message

Delete a group, given its name:

    $ ./cosmos-admin group delete
          -n, --name  <arg>
          --help          Show help message

Set the minimum quota of a group, giving its name:

    $ ./cosmos-admin group set-min-quota
          -n, --name  <arg>
          -q, --quota  <arg>
	  --help           Show help message

## Admin tab in the GUI
The Cosmos GUI shows an "Admin" tab only for those profiles with the operator capability (<code>cosmos-admin profile enable-capability -h <hadnle> -c is_operator</code>). This administration page allows operators to:

* See which are the active private computing clusters.
* Search a cluster given its identifier.
* Check the number of available computing machines.
* Put the deployment in maintenance mode.

![Admin GUI](img/admin_gui.png)

## Infinity API
The Infinity API can be seen in the [doc/infinity-api](infinity-api) file.

## Runtime configuration parameters
The configuration parameters for the different Cosmos modules can be found at `/opt/pdi-cosmos/etc/`:

* `cosmos-api.conf`
* `ial.conf`
* `logback.conf`
* `service/`
    * `global-basic.conf`
    * `hdfs.conf`
    * `hive.conf`
    * `infinity_driver.conf`
    * `infinityfs_driver.conf`
    * `infinityfs_server.conf`
    * `infinity_server.conf`
    * `mapreduce2.conf`
    * `oozie.conf`
    * `webhcat.conf`
    * `yarn.conf`
    * `zookeeper.conf` 
* `ssl/`

### Cosmos API parameters (`cosmos-api.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains a set of properties:

* Maintenance mode:

    `application.underMaintenance=false`

* Public API address:

    `application.baseurl="https://andromeda01.hi.inet"`

* Integration with Infinity:

    `infinity.secret="inf1nity_secret"`<br>
    `infinity.httpPort=51070`

* Secret key (the secret key is used to secure cryptographic functions; if you deploy your application to several instances be sure to use the same key): 

    `application.secret="qwz...wnk"`<br>
    `application.ssl.enabled=true`

* The application languages:

    `application.langs="en"`

* Application mode:

    `application.mode=prod`

* Global object class. Define the Global object class for this application. Default to Global in the root package:

    `application.global=es.tid.cosmos.api.ProductionGlobal`

* Database configuration:

    `db.default.host="localhost"`<br>
    `db.default.port=3306`<br>
    `db.default.name=cosmos`<br>
    `db.default.driver=com.mysql.jdbc.Driver`<br>
    `db.default.url="jdbc:mysql://"${db.default.host}":"${db.default.port}"/"${db.default.name}`<br>
    `db.default.user=cosmos`<br>
    `db.default.pass="SomeV3ryNicePassw0rd"`

* Related to BoneCP issue of spontaneously closing connections (see https://bugs.launchpad.net/bonecp/+bug/1243551):

    `db.default.maxConnectionAge=0 second`

* Evolutions (you can disable evolutions if needed):

    `evolutionplugin=enabled`<br>
    `applyEvolutions.default=true`<br>
    `applyDownEvolutions.default=true`

* Auth providers configuration:

    ```
    auth.pdihub {
        enabled=true
        name="PDIHub"
        class="es.tid.cosmos.api.auth.oauth2.github.Provider"
        client.id="20f0381a6698c19953d5"
        client.secret="b11b73f83e7dcf940fec3356d3d0604e3169ab98"
        auth.url="https://pdihub.hi.inet/login/oauth/"
        api.url="https://pdihub.hi.inet/api/v3/"
        signup.url="https://pdihub.hi.inet/auth/ldap/"
    }

    auth.horizon {
        enabled=true
        class="es.tid.cosmos.api.auth.external.ExternallyManagedAuthProvider"
        password="horizon!"
    }
    ```

* Link to CLI egg:

    `cli.url="https://andromeda01.hi.inet:8000/eggs/cosmos-0.17.0-py2.7.egg"`

* Play Emailer plugin:

    `smtp.host="mailhost.hi.inet"`

* Reporting and emails:

    `email.environment="andromeda"`<br>
    `email.fromHost="andromeda01"`<br>
    `email.fromAddress="noreply-cosmos-andromeda@tid.es"`<br>
    `email.reportToAddress="cosmos@tid.es"`

* Pages:

    `pages.faq.ssl.authority="TID"`<br>
    `pages.faq.ssl.certificateLocation="https://issuecatid.hi.inet/certsrv"`<br>
    `pages.faq.ssl.support.name="EPG"`<br>
    `pages.faq.ssl.support.email="epg@tid.es"`

* Swagger API documentation:

    `swagger.api.basepath=${application.baseurl}`

* Version that shows at the bottom of the Swagger Doc page:

    `api.version=1`

* IAL configuration:

    `include "ial"`

* Service manager:

    `ambari.server.url=localhost`<br>
    `ambari.server.port=8080`<br>
    `ambari.server.username=admin`<br>
    `ambari.server.password=admin`<br>
    `ambari.servicemanager.exclusiveMasterSizeCutoff=10`<br>
    `ambari.servicemanager.initialization.graceperiod.minutes=5`<br>
    `ambari.servicemanager.servicesConfigDirectory="/opt/pdi-cosmos/etc/services"`<br>
    `ambari.servicemanager.mrAppMasterMemory=1024`<br>
    `ambari.servicemanager.mapTaskMemory=2048`<br>
    `ambari.servicemanager.mapHeapMemory=1536`<br>
    `ambari.servicemanager.mappersPerSlave=8`<br>
    `ambari.servicemanager.reduceTaskMemory=3072`<br>
    `ambari.servicemanager.reduceHeapMemory=2304`<br>
    `ambari.servicemanager.reducersPerSlave=4`<br>
    `ambari.servicemanager.yarnTotalMemory=28672`<br>
    `ambari.servicemanager.yarnContainerMinimumMemory=128`<br>
    `ambari.servicemanager.yarnVirtualToPhysicalMemoryRatio=2.1`<br>
    `ambari.servicemanager.nameNodeHttpPort=50070`<br>
    `ambari.servicemanager.zookeeperPort=2181`<br>
    `hdfs.cluster.id=persistentHdfs`

* Root logger (you can also configure logback (http://logback.qos.ch/), by providing a logger.xml file in the conf directory):

    `logger.root=ERROR`

* Logger used by the framework:

    `logger.play=INFO`

* Logger provided to your application:

    `logger.application=DEBUG`

### IAL parameters (`ial.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains a set of properties:

* Private SSH key of the IAL: 

    `ial.root.ssh.private_key="""-----BEGIN RSA PRIVATE KEY-----<key>-----END RSA PRIVATE KEY-----"""`

* Json-like information about the available infrastructure needed by libvirt:

    ```ial.libvirt.servers = [
        {
            name = "<server_name>",
            description = "<server_description>",
            enabled = true,
            profile = "g1-compute",
            rack = "<virtual_rack_id_the_server_belongs_to>",
            connection = "openvz+ssh://root@<server_ip_address>/system?socket=/var/run/libvirt/libvirt-sock",
            domain_hostname = "<openvz_container_hostname>"
            domain_ipaddress = "<openvz_container_ip_address>"
        },
        {
            <add as many g1-compute objects as compute servers are available>
        },
        {
            name = "<server_name>",
            description = "<server_description>",
            enabled = true,
            profile = "hdfs-master",
            rack = "<virtual_rack_id_the_server_belongs_to>",
            connection = "openvz+ssh://root@<server_ip_address>/system?socket=/var/run/libvirt/libvirt-sock",
            domain_hostname = "<openvz_container_hostname>"
            domain_ipaddress = "<openvz_container_ip_address>"
        },
        {
            name = "<server_name>",
            description = "<server_description>",
            enabled = true,
            profile = "hdfs-slave",
            rack = "<virtual_rack_id_the_server_belongs_to>",
            connection = "openvz+ssh://root@<server_ip_address>/system?socket=/var/run/libvirt/libvirt-sock",
            domain_hostname = "<openvz_container_hostname>"
            domain_ipaddress = "<openvz_container_ip_address>"
        },
		{
			<add as many hdfs-slave objects as Infinity servers are available>
		}
    ]```

### Logback parameters (`logback.conf`)
This is [Logback](http://logback.qos.ch/) configuration file.

    <?xml version="1.0" encoding="UTF-8"?>
    <configuration>
	    <appender name="STDOUT" class="ch.qos.logback.core.ConsoleAppender">
	    <encoder>
	        <charset>UTF-8</charset>
	        <pattern>%-5level %date{dd-MM-yyyy HH:mm:ss} [%thread] [%logger{35}] - %msg%n</pattern>
	    </encoder>
	    </appender>
	
	    <appender name="FILE" class="ch.qos.logback.core.rolling.RollingFileAppender">
	        <file>/opt/pdi-cosmos/var/log/cosmos-api.log</file>
	        <rollingPolicy class="ch.qos.logback.core.rolling.TimeBasedRollingPolicy">
	            <!-- daily rollover -->
	            <fileNamePattern>infinity-server.%d{yyyy-MM-dd}.%i.log</fileNamePattern>
	            <timeBasedFileNamingAndTriggeringPolicy
	                    class="ch.qos.logback.core.rolling.SizeAndTimeBasedFNATP">
	                <!-- or whenever the file size reaches xxx MB -->
	                <maxFileSize>50MB</maxFileSize>
	            </timeBasedFileNamingAndTriggeringPolicy>
	            <!-- keep xxx days' worth of history -->
	            <maxHistory>30</maxHistory>
	        </rollingPolicy>
	        <encoder>
	           <charset>UTF-8</charset>
	           <outputPatternAsHeader>true</outputPatternAsHeader>
	           <pattern>%-5level %date{dd-MM-yyyy HH:mm:ss} [%thread] [%logger{35}] - %msg%n</pattern>
	        </encoder>
	    </appender>
	
	    <logger name="es.tid.cosmos" level="DEBUG">
	        <appender-ref ref="STDOUT" />
	    </logger>
	
	    <!-- Loggers that are reporting false errors. -->
	    <logger name="com.wordnik.swagger.play.HelpApi" level="OFF"/>
	    <logger name="com.wordnik.swagger.core.util.TypeUtil" level="OFF"/>
	
	    <root level="INFO">
	        <appender-ref ref="FILE" />
	    </root>
	</configuration>

### Services parameters (`services/`)
The following set of files configure the different Cosmos services. A default value is automatically assigned to all the parameters when a deployment is done. You can manually change any of them by editing the files.

Most of such parameters have the same name and semantic than the ones you can find in the Hadoop ecosystem. It is the case of Hadoop core, Yarn, Hive, Oozie, etc. Only for those proprietary parameters an explanation is given.

#### Global properties (`services/global-basic.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, contains a set of properties (*global*):
 
    include "cluster-properties"

    "global" {
	    "properties" {
			"<property_name>" = "<value>",
			...
		}
	}

Being the specific properties:

* `apache_artifacts_download_url`: URL regarding Apache downloads
* `ganglia_runtime_dir`: Ganglia runtime directory path
* `gmetad_user`: Unix-like username for Ganglia Meta Daemon
* `gmond_user`: Unix-like username for Ganglia Monitoring System
* `gpl_artifacts_download_url`: URL regarding GPK downloads
* `hadoop_conf_dir`: Hadoop configuration directory path
* `hadoop_heapsize`: megabytes
* `hadoop_pid_dir_prefix`: Hadoop PID directory path
* `hbase_conf_dir`: HBase configuration directory path
* `hbase_user`: Unix-like username for HBase
* `hcat_conf_dir`: HCat configuration directory path
* `hcat_user`: Unix-like username for HCat
* `hive_user`: Unix-like username for Hive
* `java64_home`: JDK directory path
* `kadmin_pw`: Kerberos admin password
* `kerberos_domain`: Kerberos domain name
* `keytab_path`: Keytabs configuration directory path
* `lzo_enabled`: LZO compression
* `nagios_group`: Nagios Unix-like group
* `nagios_user`: Unix-like username for Nagios
* `proxyuser_group`
* `rrdcached_base_dir`: RRD cache directory path
* `run_dir`
* `security_enabled`
* `smokeuser`: Unix-like username for Smoke
* `snappy_enabled`: Snappy compression
* `user_group`
* `webhcat_user`: Unix-like username for WebHCat

### HDFS properties (`services/hdfs.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, contains three sets of properties (*global*, *core*, *services*):

    include "cluster-properties"

    "global" {
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

    "core" {
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

    "services" = [{
		"configType"="hdfs-site",
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }]

Being the specific properties for *global* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/HDFS/configuration/global.xml) for a detailed explanation):

* `datanode_du_reserved`
* `dfs_datanode_address`
* `dfs_datanode_data_dir`
* `dfs_datanode_data_dir_perm`
* `dfs_datanode_failed_volume_tolerated`
* `dfs_datanode_http_address`
* `dfs_exclude`
* `dfs_namenode_checkpoint_dir`
* `dfs_namenode_checkpoint_period`
* `dfs_namenode_name_dir`
* `dfs_replication`
* `dfs_webhdfs_enabled`
* `dtnode_heapsize`
* `fs_checkpoint_size`
* `hdfs_enable_shortcircuit_read`
* `hdfs_log_dir_prefix`
* `hdfs_user`
* `namenode_formatted_mark_dir`
* `namenode_heapsize`
* `namenode_opt_maxnewsize`
* `namenode_opt_newsize`

Being the specific properties for *core* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/HDFS/configuration/core-site.xml) for a detailed explanation):

* `fs.checkpoint.size`
* `fs.defaultFS`
* `fs.trash.interval`
* `hadoop.proxyuser.oozie.groups`
* `hadoop.proxyuser.oozie.hosts`
* `hadoop.security.auth_to_local`
* `hadoop.security.authentication`
* `hadoop.security.authorization`
* `io.compression.codecs`
* `io.file.buffer.size`
* `io.serializations`
* `ipc.client.connect.max.retries`
* `ipc.client.connection.maxidletime`
* `ipc.client.idlethreshold`

Being the specific properties for *services* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/HDFS/configuration/hdfs-site.xml) for a detailed explanation):

* `dfs.block.access.token.enable`
* `dfs.blockreport.initialDelay`
* `dfs.blocksize`
* `dfs.client.read.shortcircuit`
* `dfs.client.read.shortcircuit.streams.cache.size`
* `dfs.cluster.administrators`
* `dfs.datanode.address`
* `dfs.datanode.balance.bandwidthPerSec`
* `dfs.datanode.data.dir`
* `dfs.datanode.data.dir.perm`
* `dfs.datanode.du.reserved`
* `dfs.datanode.failed.volumes.tolerated`
* `dfs.datanode.http.address`
* `dfs.datanode.ipc.address`
* `dfs.datanode.max.transfer.threads`
* `dfs.datanode.max.xcievers`
* `dfs.domain.socket.path`
* `dfs.heartbeat.interval`
* `dfs.hosts.exclude`
* `dfs.https.port`
* `dfs.journalnode.edits.dir`
* `dfs.journalnode.http-address`
* `dfs.namenode.accesstime.precision`
* `dfs.namenode.avoid.read.stale.datanode`
* `dfs.namenode.avoid.write.stale.datanode`
* `dfs.namenode.checkpoint.dir`
* `dfs.namenode.checkpoint.edits.dir`
* `dfs.namenode.checkpoint.period`
* `dfs.namenode.handler.count`
* `dfs.namenode.http-address`
* `dfs.namenode.https-address`
* `dfs.namenode.name.dir`
* `dfs.namenode.safemode.threshold-pct`
* `dfs.namenode.secondary.http-address`
* `dfs.namenode.stale.datanode.interval`
* `dfs.namenode.write.stale.datanode.ratio`
* `dfs.permissions.enabled`
* `dfs.permissions.superusergroup`
* `dfs.replication`
* `dfs.replication.max`
* `dfs.support.append`
* `dfs.webhdfs.enabled`
* `fs.permissions.umask-mode`

### Hive properties (`services/hive.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, contains three sets of properties (*global*, *core*, *services*):

    include "cluster-properties"

    "global" {
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

    "core" {
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

    "services" = [{
		"configType"="hive-site"
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }]

Being the specific properties for *global* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/HIVE/configuration/global.xml) for a detailed explanation):

* `hive_ambari_database`
* `hive_database`
* `hive_hostname`
* `hive_jdbc_connection_url`
* `hive_database_name`
* `hive_metastore_user_name`
* `hive_metastore_user_passwd`
* `hive_metastore_port`
* `hive_lib`
* `hive_conf_dir`
* `hive_dbroot`
* `hive_log_dir`
* `hive_pid_dir`
* `mysql_connector_url`
* `hive_aux_jars_path`
* `hive_aux_jars_path`
* `hive_database_type`
* `hive_jdbc_driver`

Being the specific properties for *core*:

* `hadoop.proxyuser.hive.groups`
* `hadoop.proxyuser.hive.hosts`

Being the specific properties for *services* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/HIVE/configuration/hive-site.xml) for a detailed explanation):

* `fs.file.impl.disable.cache`
* `fs.hdfs.impl.disable.cache`
* `hive.auto.convert.join`
* `hive.auto.convert.join.noconditionaltask`
* `hive.auto.convert.join.noconditionaltask.size`
* `hive.auto.convert.sortmerge.join`
* `hive.auto.convert.sortmerge.join.noconditionaltask`
* `hive.enforce.bucketing`
* `hive.enforce.sorting`
* `hive.map.aggr`
* `hive.mapjoin.bucket.cache.size`
* `hive.mapred.reduce.tasks.speculative.execution`
* `hive.metastore.cache.pinobjtypes`
* `hive.metastore.client.socket.timeout`
* `hive.metastore.execute.setugi`
* `hive.metastore.uris`
* `hive.metastore.warehouse.dir`
* `hive.optimize.bucketmapjoin`
* `hive.optimize.bucketmapjoin.sortedmerge`
* `hive.optimize.index.filter`
* `hive.optimize.mapjoin.mapreduce`
* `hive.optimize.reducededuplication`
* `hive.optimize.reducededuplication.min.reducer`
* `hive.security.authorization.enabled`
* `hive.security.authorization.manager`
* `hive.semantic.analyzer.factory.impl`
* `hive.server2.enable.doAs`
* `hive.stats.dbclass`
* `hive.stats.jdbcdriver`
* `hive.stats.dbconnectionstring`
* `hive.vectorized.execution.enabled`
* `javax.jdo.option.ConnectionDriverName`
* `javax.jdo.option.ConnectionPassword`
* `javax.jdo.option.ConnectionURL`
* `javax.jdo.option.ConnectionUserName`

### Infinity driver (`services/infinity_driver.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, contains two sets of properties (*core*, *infinity_driver*):

    include "cluster-properties"

    "core" {
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

    "infinity_driver" = {
		"configType"="infinity-driver"
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

Being the specific properties for *core*:

* `fs.infinity.impl`
* `fs.infinity.defaultAuthority`
* `es.tid.cosmos.clustersecret`

Being the specific properties for *infinity_driver*:

* `blocked_ports`: Ports not allowed to connect with the server

### Infinity FS driver (`services/infinityfs_driver.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, contains two sets of properties (*core*, *services*):

    include "cluster-properties"

    "core" {
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

    "services" = [{
		"configType"="infinity-driver"
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }]

Being the specific properties for *core*:

* `fs.infinity.impl`
* `fs.infinity.defaultAuthority`
* `fs.infinity.ssl.enabled`: SSL enabled
* `fs.infinity.clusterSecret`

Being the specific properties for *services*:

* `blocked_ports`: Ports not allowed to connect to the server

### Infinity FS server (`services/infinityfs_server.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, contains a set of properties (*infinityfs_server*):

    include "cluster-properties"

    "infinityfs_server" {
		"configType"="infinityfs-server"
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

Being the specific properties:

* `blocked_ports_master`: Ports not allowed to connect with the master
* `blocked_ports_slave`: Ports not allowed to connect with the slaves
* `server_port_master`: Master listening port 
* `server_port_slave`: Slaves listening port
* `proxy_secure_phrase_template`
* `hdfs_datanode_port`: Datanode listening port

### Infinity server properties (`services/infinity_server.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains two sets of properties (*core*, *services*):

    include "cluster-properties"

    "core" {
        "properties" {
            "<property_name>" = "<value>",
            ...
        }
    }

	"services" = [
	    {
	        "configType"="hdfs-site",
	        "properties" {
				"<property_name>" = "<value>",
	            ...
	        }
	    },
	    {
	        "configType"="infinity-server",
	        "properties" {
				"<property_name>" = "<value>",
	            ...
	        }
	    }
	]

Being the specific properties for *core*:

* `hadoop.security.group.mapping`
* `hadoop.security.group.mapping.infinity.apiBase`
* `hadoop.security.group.mapping.infinity.timeout`
* `hadoop.security.group.mapping.infinity.secret`

Being the specific properties for *services (hdfs-site)*:

* `dfs.namenode.plugins`
* `dfs.datanode.plugins`
* `dfs.infinity.apiBase`
* `dfs.infinity.secret`
* `dfs.infinity.metadata.host`
* `dfs.infinity.metadata.port`
* `dfs.infinity.metadata.replication`

Being the specific properties for *services (infinity-server)*:

* `blocked_ports`
* `ssl_certificate_content`
* `ssl_certificate_key_content`

### MapReduce 2 properties (<code>services/mapreduce2.conf</code>)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains three sets of properties (*global*, *core*, *services*):

	include "cluster-properties"

	"global" {
	    "properties" {
			"<property_name>" = "<value>",
	        ...
	    }
	}
	
	"core" {
	    "properties" {
			"<property_name>" = "<value>",
	        ...
	    }
	}
	
	"services" = [{
	    "configType" = "mapred-site",
	    "properties" {
			"<property_name>" = "<value>",
	        ...
		}
	}]

Being the specific properties for *global* (check [here](https://github.com/apache/ambari/blob/branch-1.4/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/MAPREDUCE2/configuration/global.xml) for a detailed explanation):

* `mapred_log_dir_prefix`
* `mapred_pid_dir_prefix`
* `mapred_user`

Being the specific properties for *core* (check [here](https://github.com/apache/ambari/blob/branch-1.4/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/HDFS/configuration/core-site.xml) for a detailed explanation):

* `mapreduce.jobtracker.webinterface.trusted`

Being the specific properties for *services* (check [here](https://github.com/apache/ambari/blob/branch-1.4.4/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/MAPREDUCE2/configuration/mapred-site.xml) for a detailed explanation):

* `mapreduce.admin.map.child.java.opts`
* `mapreduce.admin.reduce.child.java.opts`
* `mapreduce.admin.user.env`
* `mapreduce.am.max-attempts`
* `mapreduce.application.classpath`
* `mapreduce.cluster.administrators`
* `mapreduce.framework.name`
* `mapreduce.job.maps`
* `mapreduce.job.reduces`
* `mapreduce.job.reduce.slowstart.completedmaps`
* `mapreduce.jobhistory.address`
* `mapreduce.jobhistory.done-dir`
* `mapreduce.jobhistory.intermediate-done-dir`
* `mapreduce.jobhistory.webapp.address`
* `mapreduce.map.java.opts`
* `mapreduce.map.log.level`
* `mapreduce.map.memory.mb`
* `mapreduce.map.output.compress`
* `mapreduce.map.sort.spill.percent`
* `mapreduce.map.speculative`
* `mapreduce.output.fileoutputformat.compress`
* `mapreduce.output.fileoutputformat.compress.type`
* `mapreduce.reduce.input.buffer.percent`
* `mapreduce.reduce.java.opts`
* `mapreduce.reduce.log.level`
* `mapreduce.reduce.memory.mb`
* `mapreduce.reduce.shuffle.input.buffer.percent`
* `mapreduce.reduce.shuffle.merge.percent`
* `mapreduce.reduce.shuffle.parallelcopies`
* `mapreduce.reduce.speculative`
* `mapreduce.shuffle.port`
* `mapreduce.task.io.sort.factor`
* `mapreduce.task.io.sort.mb`
* `mapreduce.task.timeout`
* `yarn.app.mapreduce.am.admin-command-opts`
* `yarn.app.mapreduce.am.command-opts`
* `yarn.app.mapreduce.am.log.level`
* `yarn.app.mapreduce.am.resource.mb`
* `yarn.app.mapreduce.am.staging-dir`

### Oozie properties (<code>services/oozie.conf</code>)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains two sets of properties (*global*, *services*):

    include "cluster-properties"

    "global" {
	    "properties" {
			"<property_name>" = "<value>",
	        ...
	    }
	}
	
	"services" = [{
	    "configType"="oozie-site",
	    "properties" {
			"<property_name>" = "<value>",
	        ...
		}
	}]

Being the specific properties for *global* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/OOZIE/configuration/global.xml) for a detailed explanation):

* `oozie_data_dir`
* `oozie_database`
* `oozie_database_name`
* `oozie_database_type`
* `oozie_derby_database`
* `oozie_hostname`
* `oozie_jdbc_connection_url`
* `oozie_jdbc_driver`
* `oozie_log_dir`
* `oozie_metastore_user_name`
* `oozie_metastore_user_passwd`
* `oozie_pid_dir`
* `oozie_user`

Being the specific properties for *services* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/OOZIE/configuration/oozie-site.xml) for a detailed explanation):

* `oozie.authentication.kerberos.name.rules`
* `oozie.authentication.type`
* `oozie.base.url`
* `oozie.credentials.credentialclasses`
* `oozie.db.schema.name`
* `oozie.service.ActionService.executor.ext.classes`
* `oozie.service.AuthorizationService.security.enabled`
* `oozie.service.CallableQueueService.callable.concurrency`
* `oozie.service.CallableQueueService.queue.size`
* `oozie.service.CallableQueueService.threads`
* `oozie.service.HadoopAccessorService.hadoop.configurations`
* `oozie.service.HadoopAccessorService.jobTracker.whitelist`
* `oozie.service.HadoopAccessorService.nameNode.whitelist`
* `oozie.service.JPAService.create.db.schema`
* `oozie.service.JPAService.jdbc.driver`
* `oozie.service.JPAService.jdbc.password`
* `oozie.service.JPAService.jdbc.url`
* `oozie.service.JPAService.jdbc.username`
* `oozie.service.JPAService.pool.max.active.conn`
* `oozie.service.PurgeService.older.than`
* `oozie.service.PurgeService.purge.interval`
* `oozie.service.SchemaService.wf.ext.schemas`
* `oozie.service.URIHandlerService.uri.handlers` 
* `oozie.service.WorkflowAppService.system.libpath`
* `oozie.service.coord.normal.default.timeout`
* `oozie.service.coord.push.check.requeue.interval`
* `oozie.services`
* `oozie.services.ext` 
* `oozie.system.id`
* `oozie.systemmode`
* `use.system.libpath.for.mapreduce.and.pig.jobs`

### WebHcat properties (<code>services/webhcat.conf</code>)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains two sets of properties (*global*, *services*):

    include "cluster-properties"

    "global" {
	    "properties" {
			"<property_name>" = "<value>",
	        ...
	    }
	}
	
	"services" = [{
	    "configType"="webhcat-site",
	    "properties" {
			"<property_name>" = "<value>",
	        ...
		}
	}]

Being the specific properties for *global* (check [here](https://github.com/apache/ambari/blob/branch-1.2.5/ambari-server/src/main/resources/stacks/HDP/1.3.2/services/HCATALOG/configuration/global.xml) for a detailed explanation):

* `hcat_log_dir`
* `hcat_pid_dir`

Being the specific properties for *services* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/WEBHCAT/configuration/webhcat-site.xml) for a detailed explanation):

* `templeton.exec.timeout`
* `templeton.hadoop`
* `templeton.hadoop.conf.dir`
* `templeton.hcat`
* `templeton.hive.archive`
* `templeton.hive.path`
* `templeton.hive.properties` 
* `templeton.jar`
* `templeton.libjars`
* `templeton.override.enabled`
* `templeton.pig.archive`
* `templeton.pig.path`
* `templeton.port`
* `templeton.storage.class`
* `templeton.streaming.jar`
* `templeton.zookeeper.hosts`

### Yarn properties (<code>services/yarn.conf</code>)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains two sets of properties (*global*, *services*):

    include "cluster-properties"

    "global" {
	    "properties" {
			"<property_name>" = "<value>",
	        ...
	    }
	}
	
	"services" = [
		{
	    	"configType" = "yarn-site",
	    	"properties" {
				"<property_name>" = "<value>",
	        	...
			}
		},
		{
			"configType" = "capacity-scheduler",
	        "properties" {
				"<property_name>" = "<value>",
	        	...
			}
		}
	]

Being the specific properties for *global* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/YARN/configuration/global.xml) for a detailed explanation):

* `nodemanager_heapsize`
* `resourcemanager_heapsize`
* `yarn_heapsize`
* `yarn_log_dir_prefix`
* `yarn_pid_dir_prefix`
* `yarn_user`

Being the specific properties for *services (yarn-site)* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/YARN/configuration/yarn-site.xml) for a detailed explanation):

* `yarn.acl.enable`
* `yarn.admin.acl`
* `yarn.application.classpath`
* `yarn.log-aggregation-enable`
* `yarn.log-aggregation.retain-seconds`
* `yarn.log.server.url`
* `yarn.nodemanager.address`
* `yarn.nodemanager.admin-env`
* `yarn.nodemanager.aux-services`
* `yarn.nodemanager.aux-services.mapreduce_shuffle.class`
* `yarn.nodemanager.container-executor.class`
* `yarn.nodemanager.container-monitor.interval-ms`
* `yarn.nodemanager.delete.debug-delay-sec`
* `yarn.nodemanager.disk-health-checker.min-healthy-disks`
* `yarn.nodemanager.health-checker.interval-ms`
* `yarn.nodemanager.health-checker.script.timeout-ms`
* `yarn.nodemanager.linux-container-executor.group`
* `yarn.nodemanager.local-dirs`
* `yarn.nodemanager.log-aggregation.compression-type`
* `yarn.nodemanager.log-dirs`
* `yarn.nodemanager.log.retain-second`
* `yarn.nodemanager.remote-app-log-dir`
* `yarn.nodemanager.remote-app-log-dir-suffix`
* `yarn.nodemanager.resource.memory-mb`
* `yarn.nodemanager.vmem-check-enabled`
* `yarn.nodemanager.vmem-pmem-ratio`
* `yarn.resourcemanager.address`
* `yarn.resourcemanager.admin.address`
* `yarn.resourcemanager.am.max-attempts`
* `yarn.resourcemanager.hostname`
* `yarn.resourcemanager.resource-tracker.address`
* `yarn.resourcemanager.scheduler.address`
* `yarn.resourcemanager.scheduler.class`
* `yarn.resourcemanager.webapp.address`
* `yarn.scheduler.maximum-allocation-mb`
* `yarn.scheduler.minimum-allocation-mb`

Being the specific properties for *services (capacity-scheduler)* (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/YARN/configuration/capacity-scheduler.xml) for a detailed explanation):

* `yarn.scheduler.capacity.maximum-am-resource-percent`
* `yarn.scheduler.capacity.maximum-applications`
* `yarn.scheduler.capacity.root.acl_administer_queues`
* `yarn.scheduler.capacity.root.capacity`
* `yarn.scheduler.capacity.root.default.acl_administer_jobs`
* `yarn.scheduler.capacity.root.default.acl_submit_jobs`
* `yarn.scheduler.capacity.root.default.capacity`
* `yarn.scheduler.capacity.root.default.maximum-capacity`
* `yarn.scheduler.capacity.root.default.state`
* `yarn.scheduler.capacity.root.default.user-limit-factor`
* `yarn.scheduler.capacity.root.queues`
* `yarn.scheduler.capacity.root.unfunded.capacity`

#### Zookeeper properties (`services/zookeeper.conf`)
This file, written in [HOCON](https://github.com/typesafehub/config/blob/master/HOCON.md) format, basically contains a set of properties:
 
    include "cluster-properties"

    "global" {
	    "properties" {
			"<property_name>" = "<value>",
			...
		}
	}

Being the usual properties (check [here](https://github.com/apache/ambari/blob/trunk/ambari-server/src/main/resources/stacks/HDP/2.0.6/services/ZOOKEEPER/configuration/global.xml) for a detailed explanation):

* `clientPort` 
* `initLimit`
* `tickTime`
* `syncLimit`
* `zk_data_dir`
* `zk_log_dir`
* `zk_pid_dir`
* `zk_pid_file`
* `zk_user`

### SSL parameters (`ssl/`)
This folder must contain the following files:

* `cosmos_cer.pem`: Cosmos Master certificate, issued by a CA.
* `cosmos_key.pem`: SSH key used for the Cosmos Master certificate generation.
* `<ca_cer\>.pem`: Certificate of the CA itself.  