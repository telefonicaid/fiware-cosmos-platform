from fabric.api import *

env.hosts = ['192.168.56.102']
env.user = 'root'
env.password = 'staging'

def deploy_sftp():
    run("Hello dolly")
