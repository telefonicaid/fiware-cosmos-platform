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
    SockPuppet is an abstraction layer on top of the Fabric functionality. It
    aims to define a number of resources (files, packages, configurations and
    users) that must be present in a given remote host to consider that a
    system is 'deployed' in that host, and then use Fabric functions to perform
    the changes. It also aims at controlling which output from the Fabric calls
    is shown to the user, in order to have less verbose output and configurable
    logging.

    For the moment, SockPuppet only allows client code to upload a number of
    files to a remote host and then clean them up in a single cleanup call.
    """

    def __init__(self):
        self.__leftovers = []

    def upload_file(self, local_path, remote_path='~'):
        if remote_path != '~':
            if remote_path.endswith(os.sep) and not files.exists(remote_path):
                warn('creating requested directory: ~/{0}'.format(remote_path))
                run('mkdir -p ~/{0}'.format(remote_path))
            self.__leftovers.append(remote_path)
        else:
            self.__leftovers.append(os.path.join(remote_path,
                                    os.path.split(local_path)[-1]))
        put(local_path, remote_path)

    def cleanup_uploaded_files(self):
        for leftover in self.__leftovers:
            if files.exists(leftover) and len(leftover) > 0:
                run("rm -rf ~/{0}".format(leftover))
