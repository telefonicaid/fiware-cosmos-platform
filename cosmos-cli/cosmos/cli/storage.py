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

import cosmos.cli.config as c
from cosmos.storage.connection import connect
from cosmos.cli.tables import format_table
from cosmos.cli.util import ExitWithError


READ_PERM = 4
WRITE_PERM = 2
EXEC_PERM = 1

UNITS = [
    ('K', 1024),
    ('M', 1048576),
    ('G', 1073741824),
    ('T', 1099511627776),
]


def client_from_config(config):
    return connect(config.api_key, config.api_secret, config.api_url)


@c.with_config
def put_file(args, config):
    """Upload a file to HDFS"""
    client = client_from_config(config)
    target_path = client.upload_file(args.local_file, args.remote_path)
    print "%s successfully uploaded to %s" % (args.local_file.name, target_path)
    return 0


def add_put_command(subparsers):
    put_parser = subparsers.add_parser('put', help='upload file')
    put_parser.add_argument('local_file', type=argparse.FileType('r'),
                            help='local file to upload')
    put_parser.add_argument('remote_path', help='target remote path')
    put_parser.set_defaults(func=put_file)


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
    (suffix, divisor) = ('B', 1)
    for (unit, unit_size) in UNITS:
        if num_bytes >= unit_size:
            (suffix, divisor) = (unit, unit_size)
    return "%d%s" % (num_bytes / divisor, suffix)


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


@c.with_config
def ls_command(args, config):
    """List directory files"""
    client = client_from_config(config)
    listing = client.list_path(args.path)
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
    ls_parser.set_defaults(func=ls_command)


@c.with_config
def get_command(args, config):
    """Download a file from HDFS"""
    if args.local_path.endswith('/') or os.path.isdir(args.local_path):
        remote_filename = os.path.split(args.remote_path)[-1]
        target_path = os.path.join(args.local_path, remote_filename)
    else:
        target_path = args.local_path
    if os.path.isfile(target_path):
        raise ExitWithError(-1, "Local file already exists")
    client = client_from_config(config)
    with open(target_path, "wb") as out_file:
        size = client._StorageConnection__client.get_file(args.remote_path, out_file)
    print "%s bytes downloaded to %s" % (format_size(size), target_path)


def add_get_command(subparsers):
    get_parser = subparsers.add_parser(
        'get', help='list directory in the persistent storage')
    get_parser.add_argument('remote_path', help='remote path to download')
    get_parser.add_argument('local_path', help='local_path')
    get_parser.set_defaults(func=get_command)


@c.with_config
def rm_command(args, config):
    """Delete a path or path tree"""
    client = client_from_config(config)
    deleted = client._StorageConnection__client.delete_path(args.path, args.recursive)
    print "%s was %s deleted" % (args.path,
                                 "successfully" if deleted else "not")


def add_rm_command(subparsers):
    get_parser = subparsers.add_parser(
        'rm', help='delete a file or directory in the persistent storage')
    get_parser.add_argument('--recursive', '-r', action='store_true',
                            help='recursive delete')
    get_parser.add_argument('path', help='remote path to delete')
    get_parser.set_defaults(func=rm_command)


def add_storage_commands(subparsers):
    add_put_command(subparsers)
    add_ls_command(subparsers)
    add_get_command(subparsers)
    add_rm_command(subparsers)

