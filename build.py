#!/usr/bin/python2.6
"""
Module build.py

"""
from buildout import BuildOut
from optparse import OptionParser


PYPROJECTS = ['cosmos/platform/frontend/hue-apps/cosmos/']


def run():
    """
    Main entry point
    """
    parser = OptionParser()
    parser.add_option("-p", "--projects", dest="projects",
                      help="Comma-separated list of projects to build",
                      default=','.join(PYPROJECTS))
    (options, args) = parser.parse_args()

    for project in options.projects.split(','):
        BuildOut(project).build()


if __name__ == "__main__":
    run()
