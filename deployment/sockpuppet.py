"""
Module sockpuppet

sockpuppet wraps Fabric functions into its own methods to control user output
and personalize the behaviour
"""
import os
import os.path

from fabric.api import put, run
from fabric.contrib import files
from fabric.utils import warn

class SockPuppet(object):
    """
    SockPuppet is the main class for the sockpuppet module
    """

    def __init__(self):
        self.__leftovers = []

    def put(self, local_path, remote_path='~'):
        if remote_path != '~':
            if remote_path.endswith(os.sep) and not files.exists(remote_path):
                warn('creating requested directory: ~/{0}'.format(remote_path))
                run('mkdir -p ~/{0}'.format(remote_path))
            self.__leftovers.append(remote_path)
        else:
            self.__leftovers.append(os.path.join(remote_path,
                                    os.path.split(local_path)[-1]))
        put(local_path, remote_path)

    def cleanup(self):
        for leftover in self.__leftovers:
            if files.exists(leftover) and len(leftover) > 0:
                run("rm -rf ~/{0}".format(leftover))
