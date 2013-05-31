#!/bin/sh

# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.

##
# Signature: is_root_or_exit()
#
# Check whether user is root, and exit otherwise
#
function is_root_or_exit {
    if [ "$(id -u)" != "0" ]; then
        echo "Sorry, you are not root."
        exit 1
    fi
}

##
# Signature: error_and_exit(error_msg)
#
# Print error message and exit
#
function error_and_exit {
    echo $1
    exit 1
}

##
# Signature: defined_or_exit(prop_name, prop_value)
#
# Check whether given prop_value is defined. If not, indicate so and exit.
#
function defined_or_exit {
    if [ -z "$2" ]; then
        echo "Error: missing config property $1"
    fi
}

##
# Signature: file_exists_or_exit(filename)
#
# Check whether given file exists. If not, indicate so and exit.
#
function file_exists_or_exit {
    if [ ! -f "$1" ]; then
        echo "Error: file $1 doesn't exist"
        exit 1
    fi
}

##
# Signature: process_file(filename)
#
function process_file {
    # Run in sub-shell to scope the properties of input file
    (
        if [ ! -f "$input_file" ]; then
            echo "Error: missing file $input_file"
            exit 1
        fi
        /bin/echo -n "Processing $input_file... "
        source $input_file

        # Check the properties are correctly defined
        defined_or_exit "template" "$template"
        defined_or_exit "hostname" "$hostname"
        defined_or_exit "hosts_file" "$hosts_file"
        defined_or_exit "resolv_conf_file" "$resolv_conf_file"
        defined_or_exit "network_device" "$network_device"
        defined_or_exit "ip_address" "$ip_address"
        defined_or_exit "netmask" "$netmask"
        defined_or_exit "gateway" "$gateway"
        defined_or_exit "root_ssh_public_key_file" "$root_ssh_public_key_file"

        file_exists_or_exit "$hosts_file"
        file_exists_or_exit "$resolv_conf_file"
        file_exists_or_exit "$root_ssh_public_key_file"

        WORK_DIR=`mktemp -d -t cosmos-configure-template`
        TARGET_DIR="$input_file-output"
        TARGET_FILE="$TARGET_DIR/`basename $template`"
        SYSCONFIG_DIR="$WORK_DIR/etc/sysconfig"
        NETSCRIPTS_DIR="$SYSCONFIG_DIR/network-scripts"
        NETDEV_FILE="$NETSCRIPTS_DIR/ifcfg-$network_device"

        tar -C $WORK_DIR -zxf $template ||
            error_and_exit "cannot decompress template file $template"

        # Configure networking
        echo "NETWORKING=yes" > $SYSCONFIG_DIR/network
        echo "HOSTNAME=$hostname" >> $SYSCONFIG_DIR/network

        # Configure hosts file
        cp $hosts_file $WORK_DIR/etc/hosts

        # Configure resolve.conf
        cp $resolv_conf_file $WORK_DIR/etc/resolv.conf

        # Create the network script for interface
        echo "DEVICE=$network_device" > $NETDEV_FILE
        echo "BOOTPROTO=none" >> $NETDEV_FILE
        echo "ONBOOT=yes" >> $NETDEV_FILE
        echo "IPADDR=$ip_address" >> $NETDEV_FILE
        echo "NETMASK=$netmask" >> $NETDEV_FILE
        echo "GATEWAY=$gateway" >> $NETDEV_FILE
        echo "USERCTL=no" >> $NETDEV_FILE

        # Create the root public key
        ROOT_SSH_CONF_DIR="$WORK_DIR/root/.ssh"
        ROOT_SSH_AUTHORIZED_KEYS_FILE="$ROOT_SSH_CONF_DIR/authorized_keys"
        if [ ! -d "$ROOT_SSH_CONF_DIR" ]; then
            mkdir $ROOT_SSH_CONF_DIR
            chmod 700 $ROOT_SSH_CONF_DIR
        fi
        if [ ! -f "$ROOT_SSH_AUTHORIZED_KEYS_FILE" ]; then
            touch "$ROOT_SSH_AUTHORIZED_KEYS_FILE"
            chmod 600 "$ROOT_SSH_AUTHORIZED_KEYS_FILE"
        fi
        cat $root_ssh_public_key_file >> "$ROOT_SSH_AUTHORIZED_KEYS_FILE"

        # Pack again
        mkdir -p "$TARGET_DIR"
        tar -C "$WORK_DIR" -czf "$TARGET_FILE" .

        rm -rf $WORK_DIR
        echo "Done!"
    )
}

##
# Signature: main(args...)
function main {
    is_root_or_exit
    for input_file in $*; do
        process_file $input_file
    done
}

main $*
