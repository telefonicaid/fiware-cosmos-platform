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

# Setup a tunnel over ssh.
#
# The tunnel will be closed automatically when there are no more active connections.
#
# Arguments:
#  $1 pem file
#  $2 username hop host
#  $3 hop host
#  $4 remote host
#  $5 remote port (optional, default 22)
#  $6 tunnel port
#  $7 inactivity time (optional, default 10 secs)
#
function tunnel() {
  local pem="$1"
  local user="$2"
  local host="$3"
  local remote="$4"
  local port="$5"
  local lport="$6"
  local time="$7"

  if [[ -z "$port" ]]; then
    port=22
  fi

  if [[ -z "$time" ]]; then
    time=10
  fi

  ssh \
    -i $pem \
    -o StrictHostKeyChecking=no \
    -o UserKnownHostsFile=/dev/null \
    -o ExitOnForwardFailure=yes \
    -f -q \
    -L $lport:$remote:$port \
    $user@$host \
    sleep $time
}

# Execute a command over ssh.
#
# Arguments:
#  $1 pem file
#  $2 username hop host
#  $3 host
#  $4 port (optional, default 22)
#  $5 command
#
function sshexec() {
  local pem="$1"
  local user="$2"
  local host="$3"
  local port="$4"
  local command="$5"

  if [[ -z "$port" ]]; then
    port=22
  fi

  ssh \
    -i $pem \
    -o StrictHostKeyChecking=no \
    -o UserKnownHostsFile=/dev/null \
    -p $port \
    -q \
    $user@$host \
    "${command}"
}

# Copy files to a remote directory over ssh (rsync).
#
# Arguments
#  $1 pem file
#  $2 username
#  $3 host
#  $4 port (optional, default to 22)
#  $5 from directory
#  $6 to directory
#  $7 files to exclude (pattern) [optional]
#
function copy() {
  local pem="$1"
  local user="$2"
  local host="$3"
  local port="$4" # optional
  local from="$5"
  local to="$6"
  local exclude="$7" # optional

  if [[ -z "$port" ]]; then
    port=22
  fi

  if [[ -z "$exclude" ]]; then
    exclude="''"
  fi

  rsync -avz -e \
    "ssh \
       -i $pem \
       -q \
       -o StrictHostKeyChecking=no \
       -o UserKnownHostsFile=/dev/null
       -p $port" \
    --progress \
    $from \
    -p \
    --chmod=ug=rwX,o=rX \
    --exclude $exclude \
    $user@$host:$to
}

# Finds an unused port.
#
# Arguments:
#  $1 lower port range
#  $2 upper port range
#
# Returns unused port.
#
function unused_port() {
  local lower="$1"
  local upper="$2"

  for port in $(seq $lower $upper); do
    echo -ne "\035" | telnet 127.0.0.1 $port > /dev/null 2>&1;
    [ $? -eq 1 ] && break;
  done
  [ -z "$port" ] && { echo "no free local ports available"; exit 3; }
  echo "$port"
}
