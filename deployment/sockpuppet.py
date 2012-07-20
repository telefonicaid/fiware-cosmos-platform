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
        mktemp_call = run('mktemp -d')
        if mktemp_call.succeeded:
            self.__remote_tempdir = mktemp_call
            run('chmod 777 {0}'.format(self.__remote_tempdir))
        else:
            raise RuntimeError("could not create remote temporary directory")

    def get_remote_tempdir(self):
        return self.__remote_tempdir

    def upload_file(self, local_path, remote_path):
        temp_remote_path = os.path.join(self.__remote_tempdir, remote_path)
        directory = os.path.join(*os.path.split(remote_path)[:-1])
        if directory and not files.exists(directory):
                warn('creating directory: {0}'.format(directory))
                run('mkdir -p {0}'.format(directory))
        put(local_path, temp_remote_path)

    def cleanup_uploaded_files(self):
        run("rm -rf {0}".format(self.__remote_tempdir))
