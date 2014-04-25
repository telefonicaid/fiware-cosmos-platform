# Create stack

Create the stack using Openstack's heat client:

```
heat --insecure stack-create -f cosmos-stack.json \
  -P KeyName=<your-key-name> <stack-name>
```

Note that the stacks accepts more parameters. Default values are intended for
EPG environment.

Unfortunately, *Havana* Heat version lacks of some functionality required
to create a complete Cosmos stack (btw: that functionality is available in
Openstack next version, *Icehouse*). In order to complement that lack of
functionality execute the following commands:

```
neutron router-interface-add <CosmosManagementRouter> port=<CosmosManagementRouterPort>

neutron floatingip-associate <MasterPublicIP> <MasterPort>

neutron floatingip-associate --fixed-ip-address 10.0.0.102 <Store1PublicIP> <Store1Port>
neutron floatingip-associate --fixed-ip-address 10.0.0.103 <Store2PublicIP> <Store2Port>
neutron floatingip-associate --fixed-ip-address 10.0.0.104 <Compute1PublicIP> <Compute1Port>
neutron floatingip-associate --fixed-ip-address 10.0.0.105 <Compute2PublicIP> <Compute2Port>
```

Note that the required parameters are listed in stack resources tab in Horizon
GUI.

Manually modifying the stack means, among other things, that stack deletion
wont work properly if Heat detect changes on it. So before delete the stack,
you must rollback those changes:

```
neutron floatingip-disassociate <Compute2PublicIP>
neutron floatingip-disassociate <Compute1PublicIP>
neutron floatingip-disassociate <Store1PublicIP>
neutron floatingip-disassociate <Store2PublicIP>

neutron floatingip-disassociate <MasterPublicIP>

neutron router-interface-delete <ManagementRouterId> port=<ManagementPortId>
```

# Deploy to an existing stack

You'll need the following information (note that all data can be obtained from
previously created stack):

```
heat --insecure stack-show <stack-name>
```


* IPs of each host (master and slaves),
* IPs of slave vms (referred in the rest of document as `ctip`)
* IPs public (master and slaves vms)

Follow these steps:

1. Copy artifacts to each slave:

   ```
   bash scripts/deploy.sh --pem <your-pem-file.pem> \
   --user root --host <slave IP> --t-host <master-public-ip> \
   --from <dist-base-directory> --to "~/cosmos"
   --exclude "rpms"
   ```

   Please note that for Openstack based deployments, slaves are in a private
   network, thus not accessible from outside. We use `master` host to reach
   slaves hosts.

   Additional, we exclude `rpms` local directory, as it is not needed in
   slaves, and it is the heavier directory of the distro.

2. Copy artifacts to master:

   ```
   bash scripts/deploy.sh --pem <your-pem-file.pem> \
   --user root --host <master public IP> \
   --from <dist-base-directory> --to "~/cosmos"
   ```

**WARNING**: At the time of writing this document, there is no access from
Openstack to `cosmos10.hi.inet`, which is used as repo from some Cosmos
components. EPG guys are working to provide a proxy to project's repos. In
the meantime, as a work-around, you can establish a reverse ssh tunnel as
usual `ssh -R 0.0.0.0:8888:cosmos10.hi.inet:80 root@<master>` and replace
references from `openstack/common.yaml` from `cosmos10.hi.inet` to
`<master-private-ip>:8888`.

3. Bootstrap puppet on each node (master and slaves):

   ```
   bash scripts/exec.sh --pem <your-pem-file.pem> \
      --user root --host <slave IP>  --t-host <master-public-ip> \
      --command "bash ~/cosmos/scripts/puppet-bootstrap.sh"
   ```

3. Install Cosmos on each slave:

  For example, for `store1`:

   ```
   bash scripts/exec.sh --pem <your-pem-file.pem> \
   --user root --host <slave IP>  --t-host <master-public-ip> \
   --command "bash ~/cosmos/scripts/puppet-apply.sh \
     --base ~/cosmos/deployment --slave \
     --hostname store1 --environment openstack \
     --store1_cthostname <public ip>"
   ```

   Repeat for `store2`, `compute1` and `compute2`.

4. Install Cosmos in master:

   Caveat: add first master hostname to `/etc/hosts`!. (to be automated)

   ```
   bash scripts/exec.sh --pem <your-pem-file.pem> \
   --user root --host <slave IP>  --t-host <master-public-ip> \
   --command "bash ~/cosmos/scripts/puppet-apply.sh \
     --base ~/cosmos/deployment/puppet --master \
     --hostname master --environment openstack \
     --store1_cthostname <public ip> \
     --store2_cthostname <public ip> \
     --compute1_cthostname <public ip> \
     --compute2_cthostname <public ip> \
     --master_hostname <master public ip>"
   ```

Once installed you can proceed to first login.

# Using your recently installed Cosmos platform

As this installation is meant development purposes, some extra steps must be
taken in order to work properly.

Firstly, to avoid the creation of a new application in OAuth provider, this
installation reuses the api key and secret already available in `pdihub`.
Unfortunately, you must add an entry in `/etc/hosts` whith name
`master.vagrant` pointing to `MasterPublicIP`

Then, just open `http://master.vagrant` and enjoy!

# Appendix

## Preparing an image in Openstack

The Image with OpenVZ kernel was prepared from a official Centos6.4 Openstack
image. Upgrading a running instance with an OpenVZ kernel is a matter of
adding the needed Yum's `.repo` and installing the kernel image, firmware,
headers and devel.

As taking an snapshot of a VM in order to generate an image seems that don't
work properly out of the box, a solution is to cleanup the image using
`virt-sysprep`:

1. Take snapshot
    ```
    nova --show --poll image-create <<your-instance-name>> <<image-name>>
    ```
2. Download it (using glance)
    ```
    glance image-download --progress --file <<local-filename-image>> <<image-name>>
    ```
3. Cleanup the image
    ```
    virt-sysprep -a <<image-name>>
    ```
4. Update the image
    ```
    glance image-update --progress --file <<local-filename-image>> <<image-name>>
    ``
