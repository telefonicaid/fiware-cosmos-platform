<?xml version="1.0"?>
<?xml-stylesheet type="text/xsl" href="configuration.xsl"?>
<configuration>
  <property>        
    <name>dfs.name.dir</name>
    <value>${namedirs}</value>
  </property>
  <property>        
    <name>dfs.data.dir</name>
    <value>${datadirs}</value>
  </property>
  <!-- HUE integration -->
  <property>        
    <name>dfs.namenode.plugins</name>
    <value>org.apache.hadoop.thriftfs.NamenodePlugin</value>
  </property>
  <property>        
    <name>dfs.datanode.plugins</name>
    <value>org.apache.hadoop.thriftfs.DatanodePlugin</value>
  </property>
  <property>        
    <name>dfs.thrift.address</name>
    <value>${namenode}:10090</value>
  </property>
</configuration>
