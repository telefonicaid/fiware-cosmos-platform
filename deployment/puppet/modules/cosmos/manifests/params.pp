#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#

class cosmos::params (
  $thee                   = 'cosmos',
  $cosmos_basedir         = '/opt/pdi-cosmos',
  $cosmos_confdir         = '/opt/pdi-cosmos/etc',
  $cosmos_repo_url        = "http://cosmos10.hi.inet/develenv/rpms",
  $cosmos_repo_file       = "/etc/yum.repos.d/$name.repo",
  $cosmos_master          = "http://192.168.63.11",
  $cosmos_db_host         = 'localhost',
  $cosmos_db_port         = '3306',
  $cosmos_db_name         = 'cosmos',
  $cosmos_db_user         = 'cosmos',
  $cosmos_db_pass         = 'SomeV3ryNicePassw0rd',
  $tuid_auth_url          = 'https://tda-qa-07.hi.inet/',
  $tuid_api_url           = 'https://foo-test.apigee.net',
  $tuid_client_id         = 'QOGIbbuzXqYfGrgTYWZciOJ3FhpiYsfk',
  $tuid_client_secret     = '7FW6EViSbWUkv5QB',
  $cosmos_ial_key         = 'undef',
  $ambari_user            = 'admin',
  $ambari_password        = 'admin',
  $ambari_refresh_period  = '30'
) {
}
  
  

  
