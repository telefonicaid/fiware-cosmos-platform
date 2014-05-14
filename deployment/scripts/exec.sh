#!/bin/sh
#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

function usage() {
cat << EOF
Execute on remote host, optionally using a ssh tunnel.

  params: --pem <pem> --user <user> --host <host>
         [--t-pem <pem>] [--t-user <user>] [--t-host <host>]
         --command <command>

  pem: host certificate
  user: username
  host: the host

  t-pem: tunnel host certificate. optional. default to 'pem'
  t-user: tunnel username. optional. default to 'user'
  t-host: tunnel host. optional.

  command: remote command to execute
EOF
}

BASE="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# source common functions
. $BASE/functions.sh

while :; do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    --pem)
      shift; pem="$1"; shift
      ;;
    --user)
      shift; user="$1"; shift
      ;;
    --host)
      shift; host="$1"; shift
      ;;
    --t-pem)
      shift; t_pem="$1"; shift
      ;;
    --t-user)
      shift; t_user="$1"; shift
      ;;
    --t-host)
      shift; t_host="$1"; shift
      ;;
    --command)
      shift; command="$1"; shift
      ;;
    -*)
      # Unexpected option
      echo "Unexpected option. See --help for more details."
      exit 2
      ;;
    *)
      break
      ;;
  esac
done

if [[ -z "$pem" || -z "$user" || -z "$host" || -z "$command" ]]; then
  echo "Missing parameters. Try '--help' for more information."
  exit 1
fi

if [[ -z "$t_pem" ]]; then
  t_pem="$pem"
fi

if [[ -z "$t_user" ]]; then
  t_user="$user"
fi

if [[ -z "$t_host" ]]; then
  sshexec "$pem" "$user" "$host" "22" "$command"
else
  t_port=$(unused_port "20200" "20400")
  tunnel "$t_pem" "$t_user" "$t_host" "$host" "22" "$t_port"
  sshexec "$pem" "$user" "127.0.0.1" "$t_port" "$command"
fi
