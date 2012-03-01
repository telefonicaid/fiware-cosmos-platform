#!/bin/bash

if [ $# -ne  1 ]; then
	echo -e "WRONG PARAMETERS. EXPECTED: $0 RPM_UPDATE_FLAG"
	exit 1;
fi


if [ $1 -eq  0 ]; then
    root_folder="/usr/local/ps"
    rmdir $root_folder/apps/psapi

    osuser=perserver
    su - $osuser -c "rm /etc/httpd/conf.d/psapi.conf"
    sudo /etc/init.d/httpd restart
fi

exit 0;