# Class:  cosmos::params
#
#
# This is the common paramaters class for Cosmos components: 
# ambari-server
# ambari-agent

class cosmos::params (
  $thee                   = 'cosmos',
  $cosmos_env             = 'undef',
  $cosmos_version         = '1.0',
  $cosmos_basedir         = '/opt/pdi-cosmos',
  $cosmos_storage         = "${cosmos_basedir}/var/storage",
  $cosmos_conf_dir        = "${cosmos_basedir}/etc",
  $cosmos_init_dir        = "${cosmos_basedir}/etc/init.d",
  $cosmos_log_dir         = "${cosmos_basedir}/var/log",
  $cosmos_repo            = 'yes',
  $cosmos_repo_url        = "http://cosmos10.hi.inet/develenv/rpms/cosmos-deps",
  $cosmos_repo_file       = "/etc/yum.repos.d/$name.repo",
  $cosmos_master          = "http://192.168.63.11",
  $cosmos_db_host         = 'localhost',
  $cosmos_db_name         = 'cosmos',
  $cosmos_db_user         = 'cosmos',
  $cosmos_db_pass         = 'SomeV3ryNicePassw0rd',
  $cosmos_secret          = 'undef',
  $ambari_agent_ver       = 'undef',
  $ambari_server_ver      = 'undef',

) {

}
  
  

  
