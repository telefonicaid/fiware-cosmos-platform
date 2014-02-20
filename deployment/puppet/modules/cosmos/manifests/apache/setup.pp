#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#

class cosmos::apache::setup inherits cosmos::params {

  # Apache HTTPD acting as front-end server, SSL proxy and CLI repo
  class { 'apache':
    # Remove default virtual host on port 80 to allow defining a custom one
    default_vhost => false
  }
  include apache::mod::proxy_http

  $ssl_fqdn      = "${cosmos::params::master_hostname}${cosmos::params::domain}"
  # Apache module seems to require a docroot even when not applicable.
  # Using an empty folder for the SSL redirect and Cosmos-API virtual hosts
  $dummy_docroot = '/tmp/apache_null'

  apache::vhost { 'cli.repo':
    priority      => '01',
    servername    => $ssl_fqdn,
    port          => '8000',
    docroot       => $cosmos::params::cosmos_cli_repo_path,
    ssl           => true,
    ssl_cert      => $cosmos::params::ssl_cert_file,
    ssl_key       => $cosmos::params::ssl_key_file,
    ssl_ca        => $cosmos::params::ssl_ca_file,
    ssl_certs_dir => $cosmos::params::cosmos_ssl_dir
  }

  apache::vhost { 'ssl.master':
    priority        => '02',
    servername      => $ssl_fqdn,
    port            => '443',
    docroot         => $dummy_docroot,
    ssl             => true,
    ssl_cert        => $cosmos::params::ssl_cert_file,
    ssl_key         => $cosmos::params::ssl_key_file,
    ssl_ca          => $cosmos::params::ssl_ca_file,
    ssl_certs_dir   => $cosmos::params::cosmos_ssl_dir,
    sslproxyengine  => true,
    proxy_pass      => [{ 'path' => '/', 'url' => 'http://localhost:9000/' }],
    custom_fragment => 'ProxyPreserveHost On',
    request_headers => ['set X-Forwarded-Proto "https"'],
  }

  apache::vhost { 'platform.repo':
    priority      => '03',
    servername    => $ssl_fqdn,
    port          => '8081',
    docroot       => "${cosmos::params::cosmos_basedir}/rpms",
    ssl           => false,
  }

  apache::vhost { 'redirect.to.fqdn':
    priority        => '10',
    servername      => $ssl_fqdn,
    port            => '80',
    docroot         => $dummy_docroot,
    redirect_source => '/',
    redirect_dest   => "https://${ssl_fqdn}/",
    redirect_status => 'permanent',
  }

  apache::vhost { 'ssl.redirect.to.fqdn':
    priority        => '11',
    servername      => $cosmos::params::master_hostname,
    port            => '443',
    docroot         => $dummy_docroot,
    ssl             => true,
    ssl_cert        => $cosmos::params::ssl_cert_file,
    ssl_key         => $cosmos::params::ssl_key_file,
    ssl_ca          => $cosmos::params::ssl_ca_file,
    ssl_certs_dir   => $cosmos::params::cosmos_ssl_dir,
    redirect_source => '/',
    redirect_dest   => "https://${ssl_fqdn}/",
    redirect_status => 'permanent',
  }

  File[$cosmos::params::cosmos_cli_repo_path]
    -> Apache::Vhost['cli.repo', 'ssl.master', 'platform.repo', 'redirect.to.fqdn', 'ssl.redirect.to.fqdn']
    ~> Service['httpd']

  anchor{'cosmos::apache::setup::begin': }
    -> Class['apache']
    -> anchor{'cosmos::apache::setup::end': }
}
