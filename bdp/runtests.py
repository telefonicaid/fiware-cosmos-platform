"""
Module runtests.py -

Runs all unittests configured in different buildout projects.
"""
import os
import subprocess, shlex

PYPROJECTS = 'platform/frontend'

def run():
    """
    Main eintry point
    """
    projects = PYPROJECTS.split(',')
    for project in projects:
        pass
        #testresults = TestRun(project)

if __name__ == "__main__":
    run()
