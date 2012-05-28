from fabric.api import *

env.hosts = ['10.95.101.201:2222']
env.user = 'root'
env.password = 'admintid33'

def deploy_hue():
    run("yum install hue")
    run("git clone <pdi hue-fixes> <checkout dir>")
    run("git apply <hue-fixes> <hue>")
    run("scp ./cosmos/platform/frontend/hue/app/cosmos")
    # remote cd ./cosmos/platform/frontend/hue-app/cosmos
    run("python bootstrap.py")
    run("bin/buildout -c buildout.prod.cfg")
    run("/etc/init.d/frontend start")

def deploy_sftp():
    # Jenkins builds JAR
    run("scp target/* /root/injection")
    run("cat template.ini >> /root/injection/server.conf")
    run("update_config ?")
    run("/etc/init.d/injection start")
    with lcd("../../cosmos/platform/injection/server/"):
        local("mvn package")
        put("target/injection*.jar ~/injection")

def deploy_cdh():
    run("yum install cdh")

def deploy_mongo():
    pass

def deploy_models():
    pass
