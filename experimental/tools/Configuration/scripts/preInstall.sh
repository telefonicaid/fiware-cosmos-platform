#
#
# Author: Daniel Esono Ferrer 01/03/2012

#!/bin/bash
root_folder="/usr/local/ps"
mkdir -p $root_folder/apps
mkdir -p $root_folder/apps/bdp
mkdir -p $root_folder/apps/bdp/Configuration


osuser=perserver
chown $osuser:$osuser -R $root_folder/apps/

#usermod -a -G apache $osuser