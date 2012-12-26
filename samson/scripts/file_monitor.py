#!/usr/bin/python
# -*- encoding: utf-8 -*-

# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) 2012 Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.

"""
A script to detect newly created files in a given directory and send the
contents to STDOUT or a TCP/IP socket.
"""

import os
import pyinotify
import socket
import sys
from optparse import OptionParser

class DataFileReader():
    """Read a given file"""

    def __init__(self,filename):
        self.filename = ""

    def read_file(self):
        sys.stderr.write(".")
        try:
            data_file = open (self.filename, "r")
            data_file_contents = data_file.read()
        except IOError:
                sys.stderr.write("Error: Unable to find file (%s) or read its data" % data_file_str)
        finally:
            data_file.close()

        if data_file_contents:
            return data_file_contents
        else:
            return None

class SocketModHandler(pyinotify.ProcessEvent, DataFileReader):
    """Handle inotify events to be sent to a TCP/IP socket"""
    sock = None

    def __init__(self, host, port):
        self.host = host
        self.port = port

        # socket does DNS checks and will fail if the specified socket is not open
        # TODO: Add some logic handling failures
        if self.sock is None:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((host, int(port)))

    def __del__(self):
        """Clean up"""
        self.sock.shutdown(socket.SHUT_WR)
        self.sock.close()

    def process_IN_CLOSE_WRITE(self, evt):
        """Act on files that were opened for writing and have been closed"""
        self.filename = os.path.join(evt.path, evt.name)
        sys.stderr.write(".")
        data_file_contents = self.read_file()

        total_sent=0
        MSGLEN=len(data_file_contents)
        while total_sent < MSGLEN:
            sent = self.sock.send(data_file_contents[total_sent:])
            if sent == 0:
                raise RuntimeError("socket died")
            total_sent = total_sent + sent

class StdOutModHandler(pyinotify.ProcessEvent, DataFileReader):
    """Handle inotify events to be sent to STDOUT"""

    def process_IN_CLOSE_WRITE(self, evt):
        """Act on files that were opened for writing and have been closed"""
        self.filename = os.path.join(evt.path, evt.name)
        data = DataFileReader.read_file(self)

        # Write to STDOUT
        sys.stdout.write(data)

def main():
    # Executing with -h shows all the available args
    # optparse is deprecated in Python 2.7 however it's likely this script will
    # be deployed with earlier releases.
    parser = OptionParser()
    parser.add_option ("-d", "--dir", dest="data_dir",
                                         help="Look for files in DIR")
    parser.add_option ("-t", "--tcp", dest="socket",
                                         help="Send the data to host:port")
    parser.add_option ("-s", action="store_true", default=False,
                                         dest="stdout", help="Send the data to stdout")
    (options, args) = parser.parse_args()

    if options.stdout and options.socket:
        # We're not that chatty and only write to one of STDOUT or a TCP socket
        parser.error("Only one of -t/--tcp or -s can be specified")

    if not options.data_dir:
        options.data_dir = os.getcwd()

    if os.path.exists(options.data_dir):
        sys.stderr.write("Monitoring %s\n" % options.data_dir)

        if options.socket:
            (host, port) = options.socket.split(":")
            sys.stderr.write("Sending data to %s on %s\n" % (host, port))
            handler = SocketModHandler(host, port)
        else:
            # Send the output to STDOUT
            sys.stderr.write("Writing to STDOUT\n")
            handler = StdOutModHandler()

        wm = pyinotify.WatchManager()
        notifier = pyinotify.Notifier(wm, handler)
        wdd = wm.add_watch(options.data_dir, pyinotify.IN_CLOSE_WRITE)
        notifier.loop()
    else:
        sys.stderr.write("No such directory, %s\n" % options.data_dir)
        sys.stderr.write("Exiting\n")

if __name__ == "__main__":
    main()
