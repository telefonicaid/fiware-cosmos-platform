#!/bin/sh

# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.

source "`dirname $0`/functions.sh"

##
# Signature: process_file(filename)
#
function process_file {
    # Run in sub-shell to scope the properties of input file
    (
        input_file="$1"
        if [ ! -f "${input_file}" ]; then
            echo "Error: missing file ${input_file}"
            exit 1
        fi
        source ${input_file}

        # Check the properties are correctly defined
        defined_or_exit "input_template" "${input_template}"
        defined_or_exit "output_template" "${output_template}"
        defined_or_exit "hostname" "${hostname}"
        defined_or_exit "network_device" "${network_device}"
        defined_or_exit "ip_address" "${ip_address}"
        defined_or_exit "netmask" "${netmask}"
        defined_or_exit "gateway" "${gateway}"
        defined_or_exit "nameservers" "${nameservers}"

        WORK_DIR=`mktemp -d -t cosmos-conftempl_XXXX`
        TARGET_FILE="`dirname ${input_file}`/${output_template}"
        TARGET_DIR="`dirname ${TARGET_FILE}`"
        SYSCONFIG_DIR="${WORK_DIR}/etc/sysconfig"
        NETSCRIPTS_DIR="${SYSCONFIG_DIR}/network-scripts"
        NETDEV_FILE="${NETSCRIPTS_DIR}/ifcfg-${network_device}"

        tar -C "${WORK_DIR}" -zxf "`dirname ${input_file}`/${input_template}" ||
            error_and_exit "cannot decompress template file ${template}"

        # Configure networking
        echo "NETWORKING=yes" > ${SYSCONFIG_DIR}/network
        echo "HOSTNAME=${hostname}" >> ${SYSCONFIG_DIR}/network
        touch "${WORK_DIR}/etc/resolv.conf"
        for ns in ${nameservers}; do
            echo "nameserver ${ns}" >> "${WORK_DIR}/etc/resolv.conf"
        done

        # Create the network script for interface
        echo "DEVICE=${network_device}" > ${NETDEV_FILE}
        echo "BOOTPROTO=none" >> ${NETDEV_FILE}
        echo "ONBOOT=yes" >> ${NETDEV_FILE}
        echo "IPADDR=${ip_address}" >> ${NETDEV_FILE}
        echo "NETMASK=${netmask}" >> ${NETDEV_FILE}
        echo "GATEWAY=${gateway}" >> ${NETDEV_FILE}
        echo "USERCTL=no" >> ${NETDEV_FILE}

        # Pack again
        mkdir -p "${TARGET_DIR}"
        tar -C "${WORK_DIR}" -czf "${TARGET_FILE}" .

        rm -rf ${WORK_DIR}
        exit 0
    )
}

##
# Signature: main(args...)
function main {
    is_root_or_exit
    echo "(Please be patient, each process takes a while)"
    for input_file in $*; do
        echo "Processing ${input_file}... "
        (process_file ${input_file} && echo "File ${input_file} processed successfully")&
    done
    wait
}

main $*
