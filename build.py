"""
Module build.py -

Bootstraps and builds a predefined buildout project
"""
import os
import subprocess, shlex
from optparse import OptionParser

PYPROJECTS = 'cosmos/platform/frontend'

class BuildOut(object):
    """
    A BuildOut is a Python project that is based around a boostrap
    script and a buildout configuration file.
    """

    def __init__(self, project_name):
        self.project = project_name
        self.build(self.project)

    def build(self, project):
        """
        Bootstraps and then builds out a project
        """
        projects_root = os.getcwd()
        if os.path.isdir(project):
            os.chdir(project)
        else:
            print "The project %s does not exist from here: %s" %\
                  (project, projects_root)
            return False
        self.__run_subproc('python bootstrap.py', 'bootstrapping')
        self.__run_subproc('bin/buildout', 'building out')
        os.chdir(projects_root)

    def __run_subproc(self, command_string, desc):
        """
        Runs a command line string as a subprocess of this module. Requires a
        description of the subprocess for message formatting.
        """
        cmd = shlex.split(command_string)
        proc = subprocess.Popen(cmd, stderr=subprocess.STDOUT)
        proc.communicate()
        if proc.returncode != 0:
            print "Error while %s in project %s" % (desc, self.project)
        else:
            print "Success in %s project %s" % (desc, self.project)


def run():
    """
    Main eintry point
    """
    parser = OptionParser()
    parser.add_option("-p", "--projects", dest="projects",
                      help="Comma-separated list of projects to build",
                      default=PYPROJECTS)
    (options, args) = parser.parse_args()

    projects = options.projects.split(',')
    for project in projects:
        builtproject = BuildOut(project)

if __name__ == "__main__":
    run()
