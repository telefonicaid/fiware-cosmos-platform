# -*- coding: utf-8 -*-
"""
Buildout to Jenkins adapter module.

"""
import os
import os.path
import re
from shutil import copytree, rmtree
import sys
import subprocess


def copyreplace_tree(src_path, dst_path):
    """Copy a tree removing the destination path if it exists."""
    if os.path.isdir(dst_path):
        print ("Dir %s already exists. Removing..." % dst_path)
        rmtree(dst_path)
    print "Copying from %s to %s" % (src_path, dst_path)
    copytree(src_path, dst_path, symlinks=True)


class BuildOut(object):
    """
    A BuildOut is a Python project that is based around a boostrap
    script and a buildout configuration file.
    """

    DEFAULT_CONFIG = 'buildout.devel.cfg'

    def __init__(self, path, build_path=None, config=DEFAULT_CONFIG):
        self.path = os.path.abspath(path)
        self.config = config

        if build_path is None:
            self.build_path = self.path
        else:
            self.build_path = os.path.join(os.path.abspath(build_path),
                                           self.project_name())

    def copy(self):
        """Copy files to the build path."""
        if self.build_path != self.path:
            copyreplace_tree(self.path, self.build_path)

    def build(self):
        """Bootstraps and then builds out a project."""
        self.__cd_to_project()
        self.__run_subproc([sys.executable, 'bootstrap.py', '-c', self.config],
                           'bootstrapping')
        self.__run_subproc(['bin/buildout', '-c', self.config], 'building out')
        self.__cd_back()

    def run_tests(self):
        """Run NoseXUnit integrated tests."""
        self.__cd_to_project()
        self.__run_subproc([os.path.join(self.build_path, 'bin/hue'), 'test',
                            'specific', self.project_name(),
                            '--with-nosexunit'], 'Unit testing')
        if self.build_path != self.path:
            print "Bring back NoseXUnit test results"
            copyreplace_tree(os.path.join(self.build_path, 'target'),
                             os.path.join(self.path, 'target'))
        self.__cd_back()

    def project_name(self):
        return re.match(".*?([^/]*)/?$", self.path).group(1)

    def __cd_to_project(self):
        self.old_cwd = os.getcwd()
        if os.path.isdir(self.build_path):
            print "Entering %s" % self.build_path
            os.chdir(self.build_path)
        else:
            raise RuntimeError("The project path %s was not found" %
                               self.path)

    def __cd_back(self):
        print "Leaving %s" % os.getcwd()
        os.chdir(self.old_cwd)

    def __run_subproc(self, command, desc):
        """
        Runs a command line string as a subprocess of this module. Requires a
        description of the subprocess for message formatting.
        """
        print "=== %s - Exec: '%s'" % (desc, " ".join(command))
        proc = subprocess.Popen(command, stderr=subprocess.STDOUT)
        proc.communicate()
        if proc.returncode != 0:
            print "Error while %s in project %s" % (desc, self.project_name())
        else:
            print "Success in %s project %s" % (desc, self.project_name())
