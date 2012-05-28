from fabric.api import *

env.hosts = ['192.168.56.102']
env.user = 'root'
env.password = 'staging'

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

def deploy_cdh():
    run("yum install cdh")

def deploy_mongo():
    pass

def deploy_models():
    pass
