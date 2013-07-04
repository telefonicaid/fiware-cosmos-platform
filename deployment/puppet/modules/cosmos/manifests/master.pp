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

class cosmos::master {

  include stdlib, ssh_keys

  anchor { 'cosmos::master::begin': }
  ->
  class {'cosmos::base': }
  ->
  class {'cosmos::firewall_app': }
  ->
  class {'ambari': }
  ->
  class { 'mysql': }
  ->
  class { 'cosmos::master_setup': }
  ->
  class { 'cosmos::api': }
  ->
  anchor { 'cosmos::master::end': }
}
