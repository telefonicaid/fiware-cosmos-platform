"""
Fabric script file to perform common tasks on remote machines. Requires fabric[1] to be installed. 

[1] http://fabfile.org.
"""
from fabric.api import run,env

def host_type():
    run('uname -s')

def list_machines():
    run('tashi getinstances')

def list_my_machines():
    run('tashi getmyinstances')

def start_samson_bigdata():
    run('tashi createVM --name samson --cores 4 --memory 4096 --disks x86_64-natty-samson-hudson-master.qcow2:True,samson-jenkins-data.qcow2:True --nics 1001,999')

def start_samson_hudson_test_bigdata():
    run('tashi createVM --name samson-hudson-test --cores 2 --memory 2048 --disks x86_64-natty-samson-hudson-test.qcow2 --nics 999')

def start_samson_hudson_ubuntu_bigdata():
    run('tashi createVM --name samson-hudson-ubuntu --cores 2 --memory 2048 --disks x86_64-natty-samson-hudson-ubuntu.qcow2 --nics 999')

def start_samson_hudson_redhat_bigdata():
    run('tashi createVM --name samson-hudson-redhat --cores 2 --memory 2048 --disks x86_64-centos-samson-hudson-redhat.qcow2 --nics 999')

def start_samson_hudson_oneiric_bigdata():
    run('tashi createVM --name samson-hudson-oneiric --cores 2 --memory 2048 --disks samson-hudson-oneiric.qcow2 --nics 999')

def start_samson_ganglia_bigdata():
    run('tashi createVM --name samson-ganglia --cores 2 --memory 4096 --disks x86_64-natty-samson-ganglia.qcow2:True --nics 999')

def start_jenkins():
    start_samson_bigdata()
    start_samson_hudson_redhat_bigdata()
    start_samson_hudson_ubuntu_bigdata()
    start_samson_hudson_oneiric_bigdata()

"""
Start up a cluster of Ubuntu Samson nodes

e.g. start 2 nodes with the name samson-cluster-user

fab -H user@hadoop.bigdata.hi.inet start_ubuntu_cluster

e.g. start 5 nodes with a custom suffix

fab -H user@hadoop.bigdata.hi.inet start_ubuntu_cluster:count=5,name=test
"""
def start_ubuntu_cluster(name=env.user, count=2):
    run('tashi createMany --basename samson-cluster-%s --cores 2 --memory 4096 --disks x86_64-natty-samson-cluster-grant.qcow2,samson-cluster-var-samson-100G.qcow2 --count %s' % (name, count))

"""
Shutdown machines started using start_ubuntu_cluster or start_centos_cluster

e.g. Stop the "test" cluster

fab -H user@hadoop.bigdata.hi.inet stop_cluster:name=test

"""
def stop_cluster(name=env.user):
    run('tashi destroyMany --basename samson-cluster-%s' % (name))
