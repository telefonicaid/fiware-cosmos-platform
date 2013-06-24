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
# The directory containing the files to be replaced
#
REPLACEMENTS_DIR="`dirname $0`/replacements"

source "`dirname $0`/functions.sh"

##
# Signature: parse_args_or_exit(args...)
#
# Parse the arguments, setting INPUT_TEMPLATE and OUTPUT_TEMPLATE variables obtained from
# arguments. If arguments are not valid, report the error and exit.
#
function parse_args_or_exit {
    if [ "$#" -eq "2" ]; then
        INPUT_TEMPLATE="$1"
        OUTPUT_TEMPLATE="$2"
        REPL_DIR="${REPLACEMENTS_DIR}/`basename ${INPUT_TEMPLATE}`"
    else
        echo "Invalid argument count. Usage:"
        echo "   `basename $0` <input template> <output template>"
        echo ""
        exit -1
    fi
}

##
# Signature: main(args...)
#
function main {
    is_root_or_exit
    parse_args_or_exit $*
    file_exists_or_exit "${INPUT_TEMPLATE}"
    directory_exists_or_exit "${REPL_DIR}"

    WORK_DIR=`mktemp -d -t cosmos-gentempl_XXXX`

    exec_cmd "Decompressing input template" tar -C ${WORK_DIR} -zxf ${INPUT_TEMPLATE}
    exec_cmd "Copying replacement files" cp -r ${REPL_DIR}/* ${WORK_DIR}
    exec_cmd "Compressing output template" tar -C ${WORK_DIR} -czf ${OUTPUT_TEMPLATE} .
    exec_cmd "Removing temporary files" rm -rf ${WORK_DIR}
}

main $*
