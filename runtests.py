"""
Module runtests.py -

Runs all unittests configured in different buildout projects.
"""
import os
import subprocess, shlex

PYPROJECTS = 'cosmos/platform/frontend'

def run():
    """
    Main eintry point
    """
    projects = PYPROJECTS.split(',')
    projects_root = os.getcwd()
    for project in projects:
        os.chdir(project)
        cmd = shlex.split('bin/django jenkins')
        proc = subprocess.Popen(cmd, stderr=subprocess.STDOUT)
        proc.communicate()
        if proc.returncode != 0:
            print "Error while testing"
        else:
            print "Success in testing"
        os.chdir(projects_root)

if __name__ == "__main__":
    run()
