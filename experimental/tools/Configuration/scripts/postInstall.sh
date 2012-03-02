#!/bin/bash
sudo /etc/init.d/httpd restart
echo -e "\n Before using Monitoring UI you need to configure Mongo connection\n please in file /usr/local/ps/apps/bdp/Configuration/settings.py\n"