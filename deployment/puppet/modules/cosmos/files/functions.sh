#!/bin/bash

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
# Signature: dir_exists_or_exit(dirname)
#
# Check whether given directory exists. If not, indicate so and exit.
#
function directory_exists_or_exit {
    if [ ! -d "$1" ]; then
        echo "Error: directory $1 doesn't exist"
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
        exit 1
    fi
}

##
# Signature: exec_cmd(action_msg, cmd, [args...])
#
# Execute the given command with arguments, reporting the action and result in the console
#
function exec_cmd {
    /bin/echo -n "$1... "
    shift 1

    typeset cmd="$*"
    typeset retcode

    eval $cmd
    retcode="$?"
    if [ "${retcode}" == "0" ]; then
        echo "OK"
    else
        echo "FAILED!!!"
        echo "The following command returned a return code ${retcode}"
        echo "  ${cmd}"
        exit -1
    fi
}

