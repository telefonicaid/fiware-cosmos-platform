# -*- coding: utf-8 -*-
#
# Telefónica Digital - Product Development and Innovation
#
# THIS CODE AND INFORMATION ARE PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
#
# Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
# All rights reserved.
#
import argparse
import logging as log
import os.path
import time

from cosmos.cli.config import load_config
from cosmos.cli.tables import format_table
from cosmos.cli.util import ExitWithError
from cosmos.storage.connection import connect


READ_PERM = 4
WRITE_PERM = 2
EXEC_PERM = 1

UNITS = [
    ('T', 1099511627776),
    ('G', 1073741824),
    ('M', 1048576),
    ('K', 1024),
    ('B', 1),
]


class StorageCommand(object):
    """Template class for storage commands. It takes care of configuration
    loading and connection initialization.
    """

    def __init__(self, command):
        """Decorates a function that takes the CLI args, the config and a
        storage connection and returns an exit code."""
        self.__command = command

    def __call__(self, args):
        """Callable interface of this command"""
        config = load_config(args)
        conn = connect(config.api_key, config.api_secret, config.api_url)
        return self.__command(args, config, conn)


def put_file(args, config, conn):
    """Upload a file to HDFS"""
    target_path = conn.upload_file(args.local_file, args.remote_path)
    print "%s successfully uploaded to %s" % (args.local_file.name, target_path)
    return 0


def add_put_command(subparsers):
    put_parser = subparsers.add_parser('put', help='upload file')
    put_parser.add_argument('local_file', type=argparse.FileType('r'),
                            help='local file to upload')
    put_parser.add_argument('remote_path', help='target remote path')
    put_parser.set_defaults(func=StorageCommand(put_file))


def format_group(group_perm):
    """Formats an integer in the range 0-7 in unix-style.

    >>> map(format_group, [0, 5, 6, 7])
    ['---', 'r-x', 'rw-', 'rwx']
    """
    formatted  = 'r' if group_perm & READ_PERM  != 0 else '-'
    formatted += 'w' if group_perm & WRITE_PERM != 0 else '-'
    formatted += 'x' if group_perm & EXEC_PERM  != 0 else '-'
    return formatted


def format_perms(perms, file_type):
    """Formats a permission number in unix style.

    File permissions start with '-':
    >>> format_perms("640", "FILE")
    '-rw-r-----'

    While directory ones with 'd':
    >>> format_perms("711", "DIRECTORY")
    'drwx--x--x'
    """
    if file_type == "DIRECTORY":
        prefix = "d"
    else:
        prefix = "-"
    groups = [format_group(int(char)) for char in perms]
    return prefix + "".join(groups)


def format_size(num_bytes):
    """Formats a number of bytes in a human readable form by using a
    suitable unit in the order of manitude of the amount.
    >>> map(format_size, [0, 500, 1024, 502*1024*1024, 249088416*1024])
    ['0B', '500B', '1K', '502M', '237G']
    """
    if num_bytes == 0:
        return '0B'
    (unit, unit_size) = next((unit, unit_size) for (unit, unit_size) in UNITS
                             if unit_size <= num_bytes)
    return "%d%s" % (num_bytes / unit_size, unit)


def format_timestamp(timestamp):
    """Formats an UTC timestamp into a date string.
    >>> format_timestamp(1320895981256)
    'Thu, 10 Nov 2011 03:33:01'
    """
    t = time.gmtime(timestamp / 1000)
    return time.strftime("%a, %d %b %Y %H:%M:%S", t)


def format_statuses(statuses):
    """Format a list of file statuses in a human-readable table.
    >>> list(format_statuses([{ \
          "accessTime"      : 1320171722771, \
          "blockSize"       : 33554432, \
          "group"           : "supergroup", \
          "length"          : 24930, \
          "modificationTime": 1320171722771, \
          "owner"           : "webuser", \
          "pathSuffix"      : "a.patch", \
          "permission"      : "644", \
          "replication"     : 1, \
          "type"            : "FILE" \
        }, { \
          "accessTime"      : 0, \
          "blockSize"       : 0, \
          "group"           : "supergroup", \
          "length"          : 0, \
          "modificationTime": 1320895981256, \
          "owner"           : "szetszwo", \
          "pathSuffix"      : "bar", \
          "permission"      : "711", \
          "replication"     : 0, \
          "type"            : "DIRECTORY" \
       }]))
    ['-rw-r--r--  webuser   supergroup  24K  Tue, 01 Nov 2011 18:22:02  a.patch', 'drwx--x--x  szetszwo  supergroup   0B  Thu, 10 Nov 2011 03:33:01  bar    ']
    """
    return format_table([
        [format_perms(status["permission"], status["type"]),
         status["owner"],
         status["group"],
         format_size(status["length"]),
         format_timestamp(int(status["modificationTime"])),
         status["pathSuffix"]] for status in statuses],
        alignments="lllrll", separator="  "
    )


def ls_command(args, config, conn):
    """List directory files"""
    listing = conn.list_path(args.path)
    if not listing.exists:
        raise ExitWithError(404, "Directory %s not found" % args.path)
    for line in format_statuses(listing.statuses):
        print line.rstrip()
    if not listing.statuses:
        print "No directory entries"


def add_ls_command(subparsers):
    ls_parser = subparsers.add_parser(
        'ls', help='list directory in the persistent storage')
    ls_parser.add_argument('path', help='remote path to list')
    ls_parser.set_defaults(func=StorageCommand(ls_command))


def get_command(args, config, conn):
    """Download a file from HDFS"""
    (target_path, size) = conn.download_to_filename(args.remote_path,
                                                    args.local_path)
    print "%s bytes downloaded to %s" % (format_size(size), target_path)


def add_get_command(subparsers):
    get_parser = subparsers.add_parser(
        'get', help='list directory in the persistent storage')
    get_parser.add_argument('remote_path', help='remote path to download')
    get_parser.add_argument('local_path', help='local_path')
    get_parser.set_defaults(func=StorageCommand(get_command))


def rm_command(args, config, conn):
    """Delete a path or path tree"""
    deleted = conn.delete_path(args.path, args.recursive)
    print "%s was %s deleted" % (args.path,
                                 "successfully" if deleted else "not")


def add_rm_command(subparsers):
    get_parser = subparsers.add_parser(
        'rm', help='delete a file or directory in the persistent storage')
    get_parser.add_argument('--recursive', '-r', action='store_true',
                            help='recursive delete')
    get_parser.add_argument('path', help='remote path to delete')
    get_parser.set_defaults(func=StorageCommand(rm_command))


def add_storage_commands(subparsers):
    add_put_command(subparsers)
    add_ls_command(subparsers)
    add_get_command(subparsers)
    add_rm_command(subparsers)

