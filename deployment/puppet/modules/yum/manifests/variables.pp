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

class yum::variables {
  # Older versions of facter ( <1.7.1 ) don't come with $::operatingsystemmajrelease...
  $os_release_parts = split($operatingsystemrelease, '[.]')
  $operatingsystemmajrelease = $os_release_parts[0]
}
