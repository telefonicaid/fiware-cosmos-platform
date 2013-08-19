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

class cosmos::params (
  $cosmos_basedir         = '/opt/pdi-cosmos',
  $cosmos_confdir         = '/opt/pdi-cosmos/etc',
  $cosmos_cli_repo_path   = '/opt/repos',
  $cosmos_repo_deps_url   = 'http://cosmos10.hi.inet/develenv/rpms/cosmos-deps',
  $cosmos_api_mode        = 'prod',
  $cosmos_db_host         = 'localhost',
  $cosmos_db_port         = '3306',
  $cosmos_db_name         = 'cosmos',
  $cosmos_db_user         = 'cosmos',
  $cosmos_db_pass         = 'SomeV3ryNicePassw0rd',
  $tuid_auth_url          = 'https://tda-qa-07.hi.inet/',
  $tuid_api_url           = 'https://foo-test.apigee.net',
  $tuid_client_id         = 'QOGIbbuzXqYfGrgTYWZciOJ3FhpiYsfk',
  $tuid_client_secret     = '7FW6EViSbWUkv5QB',
  $cosmos_pyshop_repo     = 'http://admin:admin@cosmos10.hi.inet:8000',
  $cosmos_cli_ensure      = '0.1.dev20130709-061210',
  $ambari_user            = 'admin',
  $ambari_password        = 'admin',
  $ambari_init_period     = '5',
  $ambari_refresh_period  = '30',
  $openvz_rplcements_dir  = '/tmp/replacements',
  $openvz_targz_path      = '/tmp/replacements/centos-6-x86_64.tar.gz',
  $cosmos_private_key     = "-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAyom69Kn+cvyJKP1HzqvZ6a/DzpGH8Og2An1+Lrc3KdLWnvWR
DacLv7oA3N3WVOgTAKPk9HYjsYPFEO+115KP6PX8ygPFlLZJr6LK1FBHH0v9+F0R
w9BogHTEi7ewYgLZXOJSfMf90tZX1i5U3vX9/o2OJmSpS2ZxMb1VjRa7XdLsUHQI
9yXIorKMGG1GaUiHdhlnVblHR/mNctstSZE4LQJ17dZawoh/FjTiWVZCITlyTN9T
r4gFxiJkg5NiXMnGE4qPNV/98SrmvchyUNgBWbea4ertubO1RsafOSecoo1fQleK
EAyudbMQvDCFRWePGGVX9mGuAbH25PwBrV/vewIDAQABAoIBAQCfwpqdCUZzjy3m
DjvJExd11LYASB1R8uk/d5vD37N8oxm2E4ASo9SJrH+nuLirCtCKbb1Y86NeD9Gv
dVYbsDkiCn8XHniIwG1JRID3jkc7saaodUlhCxCTWwf6Ka/Q7+xi7wcf9GUMHRT2
Fl/N634pf6FEOZyspCbflDhabTboXuI+GEQ69l0bnAd1BoPUgOf1AIdf75s1b9jl
cQ9hRCoQmzKEJNyzPr/e/kpyN3bP14kNLccmXudvNv9NyEKNmfZdu4GXIDDB8z0X
QClv8FubtZRiNe6F9Pb7qPqqJIvWXBZ84Wd9qPMH9fOT6ocTDrUyhyZsbaDqIIt2
nxfHiTKJAoGBAPpdGzIw2oeHti75WR6vT0d2pdWbcot70D3/W0ORPiF8MR5cOtIA
rHWVFJrlXulUJ8P6wHP4qqshyXlaS7IFegyYFvpUQAFYZUja/X+qxfHUGGhTPU0e
ByoJJjD52j8JGIi0r0J9ypZl/cw5O8oXUCVz7Kl/Z7DCL+Er6mR4yZwFAoGBAM8Y
/tjteONF0OdR7WF6X/Rp/7jtJQTQ/AWDuRiUkUplnlMevb6G4ejmoQMn31Bn2xlj
GOozwcbTUi+j6qRIiB/aocX7RK65U8RpN4ZSDcjEBIcBxh80aPAVKb8YU/T9wxPT
fN4PokQIHdbEqQSpZTvoj7bNH5bKNHZc31QHnLV/AoGAUK+q8Rz/gIhDNZZUm2gC
U9SwE0xauQs5DJH8VqU7cc01g0RUDJP9AJQAJP9g/C+L9UQdvAJa9nYYGUz5cjED
qYVmelymmAhl8/nnkrp1ur8Q8qZI7k6MHob+UPBk3sXOq1gX14T0wk2yYpvbYgDr
anaWwfV2dGfQyG+/xYuL5gUCgYB2QQGUgNUcgYst6xwvIx+TgstgnxkaTPL7eyHA
JWVpI+lp8Q3Ls11gDgE2uhfL4+G591oMsY0eqSAuz9x1/mysM71YPYgUdQKeDq6D
5mq0jyk+pgapXtXk8VCrK4sVQ6mWspaEfkzbp/7R2e3hGZESylL1wN8ezUtzoshr
RQV4pQKBgQDriIKvRlgyD79BNIoOFExgqrsy4fNzgPcQwACOcVF56QfWttFyskHN
/eke0Ivuhf7JT0JQYcMNT1HmQWeaMD/aWDAsNXjcwPmfWOMuHvkjnRLBUy25WwT9
vOc96sFgQcKeKY1C7SvULGIxi+bwF1bxwZEUIn65I8Rw5qF65oasiQ==
-----END RSA PRIVATE KEY-----",
  $cosmos_raw_public_key = 'AAAAB3NzaC1yc2EAAAADAQABAAABAQDKibr0qf5y/Iko/UfOq9npr8POkYfw6DYCfX4utzcp0tae9ZENpwu/ugDc3dZU6BMAo+T0diOxg8UQ77XXko/o9fzKA8WUtkmvosrUUEcfS/34XRHD0GiAdMSLt7BiAtlc4lJ8x/3S1lfWLlTe9f3+jY4mZKlLZnExvVWNFrtd0uxQdAj3JciisowYbUZpSId2GWdVuUdH+Y1y2y1JkTgtAnXt1lrCiH8WNOJZVkIhOXJM31OviAXGImSDk2JcycYTio81X/3xKua9yHJQ2AFZt5rh6u25s7VGxp85J5yijV9CV4oQDK51sxC8MIVFZ48YZVf2Ya4Bsfbk/AGtX+97',
  # Filled by hiera data
  $cosmos_subnet,
  $cosmos_netmask,
  $cosmos_repo_platform_url,
  $master_ip,
  $master_hostname,
  $domain
) {
  $cosmos_public_key        = "ssh-rsa ${cosmos_raw_public_key} root@localhost"
  $cosmos_cli_repo          = "${cosmos_pyshop_repo}/repository/2462"
  $cosmos_cli_repo_list     = "${cosmos_pyshop_repo}/simple/cosmos"
  $cosmos_cli_filename      = resolve_cli_filename($cosmos_cli_repo_list, $cosmos_cli_ensure)

  notify {"Selected ${cosmos_cli_ensure} version of cosmos-cli is
          '${cosmos_cli_filename}'":
      withpath => true,
  }
}
