#!/usr/bin/python2.6
"""
Module build.py

"""
import sys
from optparse import OptionParser

from buildout import BuildOut


PYPROJECTS = ['cosmos/platform/frontend/hue-apps/cosmos/']


COMMANDS = {
    'build':    lambda project: project.build(),
    'runtests': lambda project: project.run_tests()
}


def run_command(command_name, projects):
    command = COMMANDS[command_name]
    for project in projects:
        command(project)


def run():
    """
    Main entry point
    """
    parser = OptionParser()
    parser.add_option("-p", "--projects", dest="projects",
                      help="Comma-separated list of projects to build",
                      default=','.join(PYPROJECTS))
    parser.add_option("-b", "--build_path", dest="build_path",
                      help="Alternative build path",
                      default=None)
    (options, args) = parser.parse_args()

    if len(args) == 0:
        print "No command to run"
        sys.exit(-1)

    for arg in args:
        if not COMMANDS.has_key(arg):
            print "Unknown command '%s'"
            sys.exit(-1)

    projects = [BuildOut(project, build_path=options.build_path) for
                project in options.projects.split(',')]
    for arg in args:
        run_command(arg, projects)


if __name__ == "__main__":
    run()
