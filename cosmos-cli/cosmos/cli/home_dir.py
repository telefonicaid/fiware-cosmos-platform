# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#
"""OS independent representation of the home directory of the user. That means
the $HOME path on unix-like systems and the "Application Data" for MS ones."""

import logging as log
import os.path
import stat
import sys


class HomeDir(object):
    """Abstract class representing the home directory"""

    def get_path(self):
        """Path of the current user home directory"""
        raise NotImplementedError()

    def read(self, filename):
        """Fully reads a file contained on the home dir"""
        with open(self.abs_path(filename)) as f:
            return f.read()

    def write(self, filename, contents):
        """Creates or replaces a file on the home dir"""
        with open(self.abs_path(filename), mode="w") as f:
            return f.write(contents)

    def abs_path(self, filename):
        """Build an absolute path from a home-relative one"""
        return os.path.join(self.get_path(), filename)

    def get_default_config_filename(self):
        """Default name of the configuration file"""
        raise NotImplementedError()

    def read_config_file(self, filename_override=None):
        filename = (filename_override if filename_override is not None
                    else self.get_default_config_filename())
        abs_path = self.abs_path(filename)
        log.info("Loading config from %s", abs_path)
        if self._is_public_file(abs_path):
            sys.stderr.write("""WARNING: Configuration file can be accessed by other users.
Please make it private ('chmod 0600 %s' in unix systems)

""" % abs_path)
        return self.read(filename)

    def _is_public_file(self, filename):
        try:
            file_mode = os.stat(filename).st_mode
            other_read = stat.S_IROTH | stat.S_IRGRP
            return file_mode & other_read > 0
        except OSError:
            # File doesn't exist
            return False

    def write_config_file(self, contents):
        filename = self.abs_path(self.get_default_config_filename())
        if not os.path.exists(filename) or _confirm_overwrite(filename):
            mode = stat.S_IRUSR | stat.S_IWUSR
            handle = os.open(filename, os.O_WRONLY | os.O_CREAT, mode)
            with os.fdopen(handle, "w") as f:
                f.write(contents)
            print "Settings saved in %s" % filename

    def get_last_cluster_filename(self):
        """Name of the file in which the last referred cluster id is stored"""
        raise NotImplementedError()

    def read_last_cluster(self):
        return self.read(self.get_last_cluster_filename())

    def write_last_cluster(self, contents):
        return self.write(self.get_last_cluster_filename(), contents)


def _confirm_overwrite(filename, default_answer=False):
    """Interactively asks for a Y/N confirmation on the console."""
    answer_text = "Y" if default_answer else "N"
    answer = raw_input("%s already exists. Overwrite? [%s]: " %
                       (filename, answer_text))
    return answer.strip().lower() in ("y", "yes")


class UnixHomeDir(HomeDir):
    """Home directory of the current user for unix boxes"""

    def __init__(self):
        self.path = os.path.expanduser("~/")

    def get_path(self):
        return self.path

    def get_default_config_filename(self):
        return ".cosmosrc"

    def get_last_cluster_filename(self):
        return ".cosmoslast"


class WinHomeDir(HomeDir):
    """Home directory of the current user for Windows boxes"""

    def __init__(self):
        self.path = os.path.join(os.getenv("USERPROFILE").decode("mbcs"),
                                 "Application Data")

    def get_path(self):
        return self.path

    def get_default_config_filename(self):
        return "cosmosrc.yaml"

    def get_last_cluster_filename(self):
        return "cosmoslast.txt"


def get():
    """Returns a HomeDir object representing the current user home directory"""
    if os.name == "nt" and os.getenv("USERPROFILE"):
        return WinHomeDir()
    else:
        return UnixHomeDir()
