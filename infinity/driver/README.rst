Infinity HFS driver
===================

HFS driver rebranding HDFS as Infinity by using a ``infinity://`` URI scheme.
Apart from that, it can be configured to point to the infinity instance of
the cluster in which Hadoop is deployed.

Build infinity-driver
---------------------

You can get a distributable JAR at
``infinity-driver/target/scala-2.10/infinity-driver.jar`` by running::

    $ sbt
    > project infinity-driver
    > assembly


Configuration
-------------

You can use infinity-driver on any cluster by adding the configuration on a per job
basis. Sample execution of the standard wordcount example:

    hadoop jar /usr/lib/hadoop/hadoop-examples.jar wordcount \
    	-Dfs.infinity.impl=es.tid.cosmos.platform.infinity.InfinityFileSystem \
    	-Dfs.infinity.defaultAuthority=foo-infinity.hi.inet \
    	-libjars infinity-driver.jar \
    	infinity:///path/to/input infinity:///path/to/output


To configure a cluster for infinity-driver you must add the same properties to the
``core-site.xml`` at Hadoop's conf::

    <property>
        <name>fs.infinity.impl</name>
        <value>es.tid.cosmos.platform.infinity.InfinityFileSystem</value>
    </property>
    <property>
        <name>fs.infinity.defaultAuthority</name>
        <value>foo-infinity.hi.inet</value>
    </property>

And add the ``infinity-driver.jar`` to ``/usr/lib/hadoop/lib`` in every host.
