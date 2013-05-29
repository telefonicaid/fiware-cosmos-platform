#!/bin/sh

# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.

if [ "$(id -u)" != "0" ]; then
	echo "Sorry, you are not root."
	exit 1
fi

if [ $# -ne 6 ]
then
  echo "Usage: `basename $0` base_template_tgz hostname hosts_file netdevice_name ip_address net_mask"
  exit $E_BADARGS
fi

TMPDIR=`mktemp -d config-templateXXX`

echo "Extracting template..."
tar -C $TMPDIR -zxf $1

echo "Customizing template..."
echo "NETWORKING=yes" > $TMPDIR/etc/sysconfig/network
echo "HOSTNAME=$2" >> $TMPDIR/etc/sysconfig/network
cp $3 $TMPDIR/etc/hosts
echo "DEVICE=$4" > $TMPDIR/etc/sysconfig/network-scripts/ifcfg-$4
echo "BOOTPROTO=none" >> $TMPDIR/etc/sysconfig/network-scripts/ifcfg-$4
echo "ONBOOT=yes" >> $TMPDIR/etc/sysconfig/network-scripts/ifcfg-$4
echo "IPADDR=$5" >> $TMPDIR/etc/sysconfig/network-scripts/ifcfg-$4
echo "NETMASK=$6" >> $TMPDIR/etc/sysconfig/network-scripts/ifcfg-$4
echo "USERCTL=no" >> $TMPDIR/etc/sysconfig/network-scripts/ifcfg-$4

echo "Recompressing template..."
tar -C $TMPDIR -czf $2.tar.gz .

echo "Cleanup..."
rm -rf $TMPDIR
