#!/usr/bin/python2.6
"""
Module build.py

"""
import sys
from optparse import OptionParser

from buildout import BuildOut


PROJECT_PATHS = ['cosmos/platform/frontend/hue-apps/cosmos/']


def run():
    """
    Main entry point
    """
    parser = OptionParser()
    parser.add_option("-b", "--build_path", dest="build_path",
                      help="Alternative build path",
                      default=None)
    (options, commands) = parser.parse_args()

    if len(commands) == 0:
        print "No command to run. Valid commands: copy, build, test"
        sys.exit(-1)

    for project_path in PROJECT_PATHS:
        project = BuildOut(project_path, build_path=options.build_path)
        if "copy" in commands:
            project.copy()
        if "build" in commands:
            project.build()
        if "test" in commands:
            project.run_tests()


if __name__ == "__main__":
    run()
