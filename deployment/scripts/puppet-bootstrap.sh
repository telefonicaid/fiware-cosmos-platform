#!/usr/bin/env bash
#
# Installs puppet.
#
# From <https://github.com/hashicorp/puppet-bootstrap>
#
# TBD: Which license? Mozilla? Telefonica-Apache?

set -e

REPO_URL="http://yum.puppetlabs.com/el/6/products/x86_64/puppetlabs-release-6-7.noarch.rpm"
VERSION=3.3.0-1.el6

if [ "$EUID" -ne "0" ]; then
  echo "This script must be run as root." >&2
  exit 1
fi

if which puppet > /dev/null 2>&1; then
  echo "Puppet is already installed."
  exit 0
fi

# Install puppet labs repo
echo "Configuring PuppetLabs repo..."
repo_path=$(mktemp)
curl -o "${repo_path}" "${REPO_URL}" 2>/dev/null
rpm -i "${repo_path}" >/dev/null

# Install Puppet...
echo "Installing puppet"
yum install -y puppet-$VERSION > /dev/null

echo "Puppet installed!"
