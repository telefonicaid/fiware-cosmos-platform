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
        cmd = shlex.split('bin/django jenkins')
        proc = subprocess.Popen(cmd, stderr=subprocess.STDOUT)
        proc.communicate()
        if proc.returncode != 0:
            print "Error while testing"
        else:
            print "Success in testing"

if __name__ == "__main__":
    run()
