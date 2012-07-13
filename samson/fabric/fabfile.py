"""
Fabric script file to perform common tasks on remote machines. Requires fabric[1] to be installed. 

[1] http://fabfile.org.
"""
from fabric.api import run,env,settings
from time import sleep
from random import randint

def host_type():
    run('uname -s')

def host_name():
    run('hostname')

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

def start_ubuntu_cluster(name=env.user):
    """
    Start up a cluster of Ubuntu Samson nodes
    """
    run('tashi createMany --basename %s --cores %s --memory %s --disks x86_64-natty-samson-cluster-grant.qcow2,samson-cluster-var-samson-100G.qcow2 --count %s --nics %s' % (env.basename, env.cores, env.memory, env.count, env.nics))

def start_centos_cluster():
    """
    Start up a cluster of CentOS Samson nodes
    """
    run('tashi createMany --basename %s --cores %s --memory %s --disks x86_64-samson-centos-cluster.qcow2 --count %s --nics %s' % (env.basename, env.cores, env.memory, env.count, env.nics))

def stop_cluster(name=env.user):
    """
    Shutdown machines started using start_ubuntu_cluster or start_centos_cluster
    e.g. Stop the "test" cluster
    fab -H user@hadoop.bigdata.hi.inet stop_cluster:name=test
    """
    run('tashi destroyMany --basename %s' % (env.basename))
    print "Waiting 5 seconds for the machines to stop"
    sleep(5)

def start_cluster(os='ubuntu', name=env.user, wait=30):
    if os == 'ubuntu':
        start_ubuntu_cluster()
    elif os == 'centos':
        start_centos_cluster()
    else:
       print 'Unknown os - %s' % (os)
    
    # Wait a bit for the machines to come up
    remaining = int(wait)
    for slot in (range(remaining, 10, -10)):
        print "Waiting %d seconds for the machines to start" % remaining
        sleep(10)
        remaining = remaining - 10

def delilah():
    # Pick a node, any node
    run("delilah -command 'cluster info'")

def configure_cluster(name=env.user):
    """
    Define the modes to be used in the cluster
    """
    print env.host

    for node in (range(int(count))):
        print node

def tashi(user=env.user, public_ip=False, count=2, name=None, cores=2, nics=999, mem=2048):
    """
    Config for connecting to hadoop.bigdata.hi.inet
    """
    env.hosts = ['hadoop.bigdata.hi.inet']
    env.user = user
    if public_ip != False:
        env.public_ip = True
    else:
        env.public_ip = False
    env.count = count

    if name == None:
        env.basename = "samson-cluster-%s" % user
    else:
        env.basename = name

    env.cores = cores
    env.nics = nics
    env.memory = mem


def samson(user=env.user,password=None):
    """
    Config for connecting to samson.bigdata.hi.inet
    """
    env.hosts = ['samson.bigdata.hi.inet']
    env.user = user
    if password != None:
        env.password = password

def samson_node():
    master=randint(0,int(env.count)-1)

    env.hosts = ['samson@%s-%d' % (env.basename, master)]
    env.password = 'samson'

def start_svn_node(branch="/trunk", port=1324, web_port=1202, traces=None):
    """
    (re)start samsonWorker from an SVN build area
    """
    # Ingore errors
    with settings(warn_only=True):
        run('killall samsonWorker')
        run('make -f src/samson/%s/makefile clear_ipcs'  % (branch))
        run('. src/samson/%s/scripts/buildenv.sh && rm -rf $SAMSON_WORKING/*' % (branch))

    if traces == None:
       run('. src/samson/%s/scripts/buildenv.sh && samsonWorker -port %s -web_port %s' % (branch, port, web_port))
    else:
       run('. src/samson/%s/scripts/buildenv.sh && samsonWorker -port %s -web_port %s -t %s' % (branch, port, web_port, traces))
