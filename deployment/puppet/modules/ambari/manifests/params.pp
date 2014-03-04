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

class ambari::params (
  $repo_url,
  $hdp_stack_repo_url,
  $reposync,
  $cosmos_stack_repo_url,
  $hdp_utils_repo_url,
  $hdp_utils_gpg_url,
  $install_ensure  = 'latest',
  $jdk_url,
  $jce_url
) {
}
