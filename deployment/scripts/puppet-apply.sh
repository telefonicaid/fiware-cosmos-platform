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
  usage: --base <basedir> {--master|--slave} [--hostname <hostname>]
    --environment <environment>
    [--<hostname>_hostname <hostname or public ip>]
    [--<hostname>_cthostname <hostname or public ip>]

  where

    basedir: location of cosmos puppet files
    flavor: this instance flavor 'master' or 'slave'
    environment: which hiera environment data to use
    hostname: to match with hiera hostname if real name is different

    <hostname>_hostname: public ip or hostname (only for master)
    <hostname>_cthostname: public ip or hostname (only for slaves)
EOF
}

while :; do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    --base)
      shift; base="$1"; shift
      ;;
    --environment)
      shift; environment="$1"; shift
      ;;
    --master)
      flavor="master"; shift
      ;;
    --slave)
      flavor="slave"; shift
      ;;
    --hostname)
      shift; export FACTER_hostname="$1"; shift
      ;;
    --*)
      val=${1:2}
      shift; export FACTER_${val}=$1; shift
      ;;
    *)
      break
      ;;
  esac
done

if [[ -z "$base" || -z "$flavor" || -z "$environment" ]]; then
  echo "Missing parameters. Try '--help' for more information."
  exit 1
fi

puppet apply \
  --modulepath=${base}/puppet/modules:${base}/puppet/modules_third_party:${base}/rpms \
  --manifestdir=${base}/puppet/modules/cosmos/manifests \
  --verbose --ignorecache --show_diff --no-usecacheonfailure \
  --environment $environment \
  --hiera_config ${base}/puppet/modules/cosmos/manifests/hiera.yaml \
  --detailed-exitcodes \
  ${base}/puppet/modules/cosmos/manifests/site.pp
