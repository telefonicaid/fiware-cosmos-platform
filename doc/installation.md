# Cosmos installation

## Requirements

### Operating system requirements

Whichever 64 bits Linux distribution can be used while supporting OpenVZ and Puppet (see Dependencies section),
but CentOS 6.4 is recommended. 

### Resource requirements

Cosmos platform requires the following 3 machines:

- The master node machine, supporting the APIs and all the management logic of the platform.
- The Infinity namenode machine, supporting the naming of all the files within the permanent storage of Cosmos.
- The Infinity datanode machine, supporting the real data of the files managed by the Infinity namenode machine.

In addition, at least 2 slave machines are required, in order to host the cluster's nodes. The exact number of
slave machines depends on your needs, the many slaves you deploy, the many clusters you will be able to create. 

These machines may be physical (recommeded) or virtualized on top on some hipervisor-based virtualization tool
such as [VirtualBox](www.virtualbox.org) or [VMWare](www.vmware.com). Anyway, the following hardware is
recommended:

- 32 GB RAM 
- At least 500 GB HDD 

When formating the file system of the machines (except the one acting as master node), take into account that a
specific partition for OpenVZ (/vz) is desirable (format it to ext4). 

## Dependencies

In addition to the common tools not present in a fresh installation of CentOS (wget, unzip, git, etc.), the
installation basically depends on Puppet and OpenVZ. 

### Puppet

In order to install Puppet in all the machines, just add the Puppet repository: 
    
    $ rpm -ivh http://yum.puppetlabs.com/el/6/products/i386/puppetlabs-release-6-7.noarch.rpm


and install the package: 

    $ yum install puppet


### OpenVZ

The OpenVZ kernel must be installed in all the machines except the master one. 

In order to proceed, download and install vzkernel-firmware and vzkernel version 2.6.32-042stab079.6 (this
version is **mandatory**; it is highly recommended not to install the OpenVZ repository) from
https://openvz.org/Download/kernel:

    $ wget http://download.openvz.org/kernel/branches/rhel6-2.6.32/042stab079.6/vzkernel-firmware-2.6.32-042stab079.6.noarch.rpm
    $ wget http://download.openvz.org/kernel/branches/rhel6-2.6.32/042stab079.6/vzkernel-2.6.32-042stab079.6.x86_64.rpm
    $ rpm -Uvh vzkernel-firmware-2.6.32-042stab079.6.noarch.rpm
    $ rpm -Uvh vzkernel-2.6.32-042stab079.6.x86_64.rpm


After installing, disable SELinux and reboot the machine in order the installation applies. **This step is
recommended for the master node** as well. 

    $ echo "SELINUX=disabled" > /etc/sysconfig/selinux
    $ shutdown -r now


The rest of the steps in a typical [OpenVZ installation](http://openvz.org/Quick_installation) are automatically
performed by the Puppet scripts. 

## Installation and configuration

Let's suppose the following machines configuration: 

- Master node
    - Hostname: cosmos-master (this hostname must be used for the cert creation in Step 3)
    - IP address: ip1 
- Infinity namenode
    - Hostname: machine02
    - IP address: ip2 
- Infinity datanode 
    - Hostname: machine03 
    - IP address: ip3 
- Slave 1
    - Hostname: machine04
    - IP address: ip4
- Slave 2
    - Hostname: machine05
    - IP address: ip5
- Slave N
    - Hostname: machine(3+N)
    - IP address: ip(3+N) 


All the machines are in the "my_domain" domain and in the "my_subnet/my_netmask" subnet. There is a gateway
which IP address is "my_gateway". 

It is highly recommended the hostname resolution is added to /etc/hosts in all the machines. 

The installation is automated thanks to Puppet: 

### Step 1: Cosmos platform installation software download

Log into the machine acting as Master node, download the Cosmos ZIP file into a temporal folder (COSMOS_TMP_PATH)
and uncompress it: 

    $ mkdir [COSMOS_TMP_PATH]
    $ cd [COSMOS_TMP_PATH]
    $ wget http://130.206.81.65/cosmos/cosmos-platform/cosmos-platform_2.10-[COSMOS_VERSION].zip
    $ unzip cosmos-platform_2.10-0.16.0.zip


Once unzipped, you will find two subfolders, `[COSMOS_TMP_PATH]/puppet` and `[COSMOS_TMP_PATH]/rpms`. The first
one contains all the necessary scripts to deploy the software, and it must be copied to all the machines (it
is recommended to be be copied once the configuration has been set up in the master, thus such configuration
can be replicated in the slaves). The second subfolder contains the RPM packages necessary to install the
Master node, and is not necessary to be copied to the Slave nodes. 

### Step 2: platform environment creation

Create the following folders in all the machines: 

    $ mkdir [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>
    $ mkdir -p [COSMOS_TMP_PATH]/puppet/modules/cosmos/files/environments/<my_environment>/certs


### Step 3: generating and installing the master node's certificates

The platform requires a certificate for the master node, signed by a valid CA, is installed in order to be
shown as an authentication proof. Thus, this certificate must be created by generating a certificate request
(do it once in the master node): 

    $ openssl req -newkey rsa:2048 -new -keyout newkey.pem -out newreq.pem


The above command will prompt for certain information; the most important information regarding the Cosmos
platform is the name of the server (whichever hostname was chosen for the cosmos master node) where the
certificate is going to be installed, and that the challenge password must be empty. Althought the PEM pass
phrase must be empty (otherwise, the httpd server will not be automatically started), it has to be filled in
this step and removed later by performing:

    $ openssl rsa -in newkey.pem -out newkey.pem


The content of the generated 'newreq.pem' file must be used within the CA in order to retrieve the final
certificate, which will be typically called 'certnew.cer'. Please rename this file and the 'newkey.pem' and
newreq.pem according to this (do it in all the machines): 

    $ cp newkey.pem [COSMOS_TMP_PATH]/puppet/modules/cosmos/files/environments/<my_environment>/certs/<cosmos-master-node>_key.pem
    $ cp cernew.cer [COSMOS_TMP_PATH]/puppet/modules/cosmos/files/environments/<my_environment>/certs/<cosmos-master-node>_cer.pem
    $ cp newreq.pem [COSMOS_TMP_PATH]/puppet/modules/cosmos/files/environments/<my_environment>/certs/<cosmos-master-node>_req.pem


### Step 4: CA's certificate installation

The CA's certificate itself must be installed. Download it from the appropiate link and do the following in
the Cosmos master node: 

Copy the CA's certificate to the local certificates store and change directory to it: 

    $ mv <ca_cert>.pem /etc/pki/tls/certs
    $ cd /etc/pki/tls/certs


Create a symbolic link for the CA's certificate. An 8-digit-number-based file will be created. It is very
important the extension of this file is '.0': 

    $ ln -s <ca_cert>.pem `openssl x509 -hash -noout -in <ca_cert>.pem`.0


Verify the certificate has ben successfully installed: 

    $ openssl verify -CApath /etc/pki/tls/certs <ca_cert>.pem
    xxxxxxxx.0: OK

### Step 5: creating the containers identity

For each slave, its SSH identity is stored in the default file /etc/ssh/ssh_host_rsa_key(.pub). Something
similar must be generated for the container (no passphrase must be used): 

    $ ssh-keygen -t rsa
    Generating public/private rsa key pair.
    Enter file in which to save the key (/root/.ssh/id_rsa): /etc/ssh/ssh_ct_rsa_key
    Enter passphrase (empty for no passphrase):
    Enter same passphrase again:
    Your identification has been saved in /etc/ssh/id_rsa.
    Your public key has been saved in /etc/ssh/id_rsa.pub.
    The key fingerprint is:
    91:6e:e3:c4:62:53:6b:2a:cb:fb:e3:8c:5f:bc:c9:c3 root@cosmos1.hi.inet
    The key's randomart image is:
    +--[ RSA 2048]----+
    |                 |
    |         .       |
    |        +        |
    |       + o       |
    |      + S        |
    |     . X .       |
    |    . ..+        |
    |   . =.oEo       |
    |    ===.+.       |
    +-----------------+


Now, copy all the slave and containers SSH private keys to the environment folder of all the machines: 

    $ cp /etc/ssh/ssh_host_rsa_key [COSMOS_TMP_PATH]/puppet/modules/cosmos/files/environments/<my_environment>/machine0i_key
    $ cp /etc/ssh/ssh_ct_rsa_key [COSMOS_TMP_PATH]/puppet/modules/cosmos/files/environments/<my_environment>/machine0i_ct_key


### Step 6: Puppet configuration

Proceed to configure the following Puppet files in all the machines (please observe there is not a
machine01.yaml file since it is the master node, not a slave). 

Do it only once in the master node and then copy the [COSMOS_TMP_PATH]/puppet folder in all the slaves. Do
not copy [COSMOS_TMP_PATH]/rpms since they are only needed in the master node. 

- `[COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>/common.yaml` 

```yaml
 # [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>/common.yaml
 # do not change
 ambari::params::repo_url: 'http://130.206.81.65/cosmos/ambari/'
 # should not be changed
 ambari::params::hdp_stack_repo_url: 'http://public-repo-1.hortonworks.com/HDP/centos6/'
 # do not change
 ambari::params::cosmos_stack_repo_url: 'http://%{cosmos::params::master_ip}:%{cosmos::params::master_repo_port}/'
 # do not change
 ambari::params::jdk_url: 'http://130.206.81.65/cosmos/java/jdk-6u31-linux-x64.bin'
 # do not change
 ambari::params::jce_url: 'http://130.206.81.65/cosmos/java/jce_policy-6.zip'
 # should not be changed
 ambari::params::hdp_utils_repo_url: 'http://public-repo-1.hortonworks.com/HDP-UTILS-1.1.0.16/repos/centos6/'
 # do not change
 ambari::params::hdp_utils_gpg_url: 'http://130.206.81.65/cosmos/ambari/RPM-GPG-KEY-Jenkins'
 # put your region for timezone configuration
 timezone::region: 'Europe'
 # put your city for timezone configuration
 timezone::locality: 'Madrid'
 # should not be changed
 yum::params::yum_epel: 'http://dl.fedoraproject.org/pub/epel/6/x86_64/'
 # should not be changed
 yum::params::yum_centos: 'http://mirror.centos.org/centos/6/os/x86_64/'
 # should not be changed
 yum::params::yum_centos_updates: 'http://mirror.centos.org/centos/6/updates/x86_64/'
 # not used
 yum::params::yum_redhat: 
 # not used
 yum::params::yum_redhat_optional: 
 # should not be changed
 yum::params::yum_puppet: 'http://yum.puppetlabs.com/el/6/products/x86_64/'
 # should not be changed
 yum::params::yum_puppet_deps: 'http://yum.puppetlabs.com/el/6/dependencies/x86_64/'
 # do not change
 cosmos::params::cosmos_repo_deps_url: 'http://130.206.81.65/cosmos/rpms/cosmos-deps'
 # do not change
 cosmos::openvz::images::base_image_url: 'http://130.206.81.65/cosmos/repos/ovz-templates'
 # do not change
 cosmos::openvz::images::image_name: 'centos-6-cosmos.HDP.2.0.6-20140123-x86_64.tar.gz'
 # put the subnet your machines are
 cosmos::params::cosmos_subnet: 'my_subnet'
 # put the mask for the above subnet
 cosmos::params::cosmos_netmask: '255.255.0.0'
 # do not change
 cosmos::params::cosmos_ssl_cert_source: "puppet:///modules/cosmos/environments/%{environment}/certs/%{hostname}_cer.pem"
 # do not change
 cosmos::params::cosmos_ssl_key_source: "puppet:///modules/cosmos/environments/%{environment}/certs/%{hostname}_key.pem"
 # put the gateway IP address for your subnet
 cosmos::slave::gateway: 'my_gateway'
 # should be the same than the above
 cosmos::slave::ct_gateway: 'my_gateway'
 # do not change
 cosmos::slave::host_key_priv_file: "puppet:///modules/cosmos/environments/%{environment}/%{hostname}_key"
 # do not change
 cosmos::slave::ct_key_priv_file: "puppet:///modules/cosmos/environments/%{environment}/%{hostname}_ct_key"
 # put the domain your machines are
 cosmos::params::domain: '.my_domain'
 # put the master node IP address
 cosmos::params::master_ip: 'ip1'
 # for future usage, do not change
 cosmos::params::master_repo_port: '8081'
 # put the master node hostname, must be always cosmos-master
 cosmos::params::master_hostname: 'cosmos-master'
 # do not change
 cosmos::params::pdihub_client_id: '20f0381a6698c19953d5'
 # do not change
 cosmos::params::pdihub_client_secret: 'b11b73f83e7dcf940fec3356d3d0604e3169ab98'
 # do not change
 cosmos::params::horizon_password: 'horizon!'
 cosmos::params::ssl_authority: 'TID'
 cosmos::params::ssl_cert_location: 'https://issuecatid.hi.inet/certsrv'
 cosmos::params::ssl_support_name: 'EPG'
 cosmos::params::ssl_support_email: 'epg@tid.es'
 cosmos::params::ambari_mr_app_master_memory: 1024
 cosmos::params::ambari_map_task_memory: 2048
 cosmos::params::ambari_map_heap_memory: 1536
 cosmos::params::ambari_reduce_task_memory: 3072
 cosmos::params::ambari_reduce_heap_memory: 2304
 cosmos::params::ambari_yarn_total_memory: 4096
 cosmos::params::ambari_yarn_container_min_memory: 128
 cosmos::params::ambari_yarn_virtual_physical_memory_ratio: 2.1
 # put the network interface name
 cosmos::openvz::network::host_iface: 'eth0'
 # put the list of slave machines
 slave_hosts:
     - 'slaveone'
     - 'slavetwo'
     - 'slavethree'
     - 'slavefour'
```

- `[COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>/machine02.yaml`
- `[COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>/machine03.yaml`
- `[COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>/machine04.yaml` 
- `[COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>/machineN.yaml` 

```yaml
 # [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hieradata/<my_environment>/machine0i.yaml
 # do not change
 cosmos::slave::rack: "rack01"
 # put the slave IP address
 cosmos::slave::ip: "ip_i"
 # put the container IP address
 cosmos::slave::ct_ip: "ip_ct_i"
 # put the the hostname the container will have, typically "my_environment-machine0i"
 cosmos::slave::ct_hostname: "my_environment-machine0i"
 # put the hardware profile, "hdfs-master" for the Infiity namenode, "hdfs-slave" for the Inifinity datanode, "g1-compute" for the rest
 cosmos::slave::hardware_profile: g1-compute
 # put the slave hostname
 cosmos::slave::name: "machine0i"
 # put the RAM memory of the slave
 cosmos::slave::ram: 32768
 # put the SSH identity of the slave, i.e. /etc/ssh/ssh_host_rsa_key.pub
 cosmos::slave::host_key_pub: "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQCgVroeZLdAYPafPW8S3HBXcMzhexrmg3VxGYsvtFgTi2Baapw89BYy+XtDGGWrHPddcGRW5ZkwueIpvGE7z1cv42LqRWEnm6ZkWAgCKUPI8O/rqFLI3BX1OOXpwxNcdeJhbhgH9LTC56WPHPvkMjtScZiOcB2V+ptJcS0fMOgNX/kA/IGSOH2owSwBe4/Xm3E79H0Fkot77koIbjBpDz6MFYrJRIIQVuKzNrrox1rnGlON5HCc7ZwLO0Zw+aozjUFliAeyTFshCB2tvex//2JN5RiCvqXIt9vXdJBj/dp13nZu9r/XET3H2IVHsHan4k4SaclpSIdt2BRHqPbcBQbL"
 # put the SSH identity of the container, i.e. /etc/ssh/ssh_ct_rsa_key.pub
 cosmos::slave::ct_key_pub: "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABAQC2LClrbl8UVATFMxa/lkUyTvDVEMm3VwX5rsjn/30MfyZX5llAa/jaSg50B/HKrh8gT0jNfXBO2v114GXbbHI1kQ6NafzmsqY0qZkNtpEu21D0NwkjOSEZXpokZaQ94DSc2boI2oRojoYTZMrvgv/4T7nyT83q+hPSiIyVxUGqQtHRZ02+hkwSquhOg02kExhsQtEj0V5RjHhXOlwnY4Ip4R5BXtDwGGwMIOaYtedT8CMpghZsV6xUAmd8yqvYRNjYJdpPY5xOeKF1weL7g/T4ZlEGN8PK3Grl1E2+dDtJ/1mFTLtOBtfgVggvXuM0QC0cXV12tERxGYzMda1lH/Sz"
```

and 

- `[COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/nodes/<my_environment>.pp `

```puppet
 # do not change
 node 'cosmos-master' {
   include profiles::master
 }
 
 # put all the slave machines
 node /^machine(0[^1]|[^0].)$/ {
   include profiles::slave
 }
```

### Step 7: applying Puppet

Start with the slaves, and apply Puppet in the server at the end. 

Slaves command: 

```bash
 $ puppet apply --debug --verbose \
   --modulepath [COSMOS_TMP_PATH]/puppet/modules/:[COSMOS_TMP_PATH]/puppet/modules_third_party/ \
   --environment fiware --hiera_config [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hiera.yaml \
   --manifestdir [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/ [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/site.pp \
   > puppet.out 2> puppet.err
```

Please observe the connection with the slave is lost, and that it is only reachable from the master
node (the IP address of the slave is now binded to the OpenVZ bridging interface). 

Server command (the same then above, but adding the rpms folder to the modulepath option): 

```bash
 $ puppet apply --debug --verbose \
   --modulepath [COSMOS_TMP_PATH]/puppet/modules/:[COSMOS_TMP_PATH]/puppet/modules_third_party/:[COSMOS_TMP_PATH]/rpms/ \
   --environment fiware --hiera_config [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/hiera.yaml \
   --manifestdir [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/ [COSMOS_TMP_PATH]/puppet/modules/cosmos/manifests/site.pp \
   > puppet.out 2> puppet.err
```