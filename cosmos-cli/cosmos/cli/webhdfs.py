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
from urlparse import urlparse, urljoin

import requests

import cosmos.cli.config as c
from cosmos.cli.util import ExitWithError, ResponseError
from cosmos.cli.tables import format_table
from cosmos.common.routes import Routes

SUPPORTED_VERSIONS = [1]

READ_PERM = 4
WRITE_PERM = 2
EXEC_PERM = 1

UNITS = [
    ('K', 1024),
    ('M', 1048576),
    ('G', 1073741824),
    ('T', 1099511627776),
]

BUFFER_SIZE = 4096


class WebHdfsClient(object):

    def __init__(self, webhdfs_uri, username, client=requests):
        self.webhdfs_uri = webhdfs_uri
        self.username = username
        self.client = client

    def opParams(self, operation, **kwargs):
        return dict(kwargs.items() + [
            ('op', operation),
            ('user.name', self.username),
        ])

    def put_file(self, local_file, remote_path):
        datanode_url = self.__request_upload_location(remote_path)
        response = self.client.put(datanode_url, data=local_file)
        if response.status_code != 201:
            raise ResponseError('Cannot upload file to %s' % datanode_url,
                                response)

    def __request_upload_location(self, remote_path):
        response = self.client.put(self.to_http(remote_path),
                                   allow_redirects=False,
                                   params=self.opParams('CREATE'))
        if response.status_code == 201:
            raise ExitWithError(
                -1, "WebHDFS uploads are not supported on 1-machine " +
                "clusters: an empty file has been created. See #862.")
        if self.__is_replication_exception(response):
            raise ExitWithError(500, 'Cannot replicate file %s blocks' %
                                remote_path)
        if response.status_code != 307:
            raise ResponseError('Not redirected by the WebHDFS frontend',
                                response)
        return response.headers['Location']

    def list_path(self, path):
        """Lists a directory or check a file status. Returns a list of status
        objects as defined in
        http://hadoop.apache.org/docs/r1.0.4/webhdfs.html#FileStatus
        or None if the path does not exits.
        """
        r = self.client.get(self.to_http(path),
                            params=self.opParams('LISTSTATUS'))
        if r.status_code == 200:
            return r.json()["FileStatuses"]["FileStatus"]
        elif r.status_code == 404:
            return None
        else:
            raise ResponseError('Cannot list directory %s' % path, r)

    def path_type(self, path):
        """Checks the type of a path returning either 'DIRECTORY', 'FILE' or
        'NONE'."""
        status = self.list_path(path)
        if status is None:
            return 'NONE'
        if len(status) == 1 and status[0]["pathSuffix"] == '':
            return status[0]["type"]
        return 'DIRECTORY'

    def get_file(self, remote_path, out_file):
        r = self.client.get(self.to_http(remote_path), stream=True,
                            params=self.opParams('OPEN'))
        if r.status_code == 404:
            raise ExitWithError(404, 'File %s does not exist' % remote_path)
        if r.status_code != 200:
            raise ResponseError('Cannot download file %s' % remote_path, r)
        buf = r.raw.read(BUFFER_SIZE)
        written = 0
        while (len(buf) > 0):
            written += len(buf)
            out_file.write(buf)
            buf = r.raw.read(BUFFER_SIZE)
        return written

    def delete_path(self, path, recursive=False):
        r = self.client.delete(self.to_http(path), params=self.opParams(
            'DELETE', recursive=str(recursive).lower()))
        if r.status_code != 200:
            raise ResponseError('Cannot delete path %s' % path, r)
        return r.json()["boolean"]

    def to_http(self, path):
        """Translates a simple path to the related WebHDFS HTTP URL."""
        if path.startswith("/"):
            rel_path = path[1:]
        else:
            rel_path = path
        return urljoin('http://' + urlparse(self.webhdfs_uri).netloc,
                       '/webhdfs/v1/user/%s/%s' % (self.username, rel_path))

    def __is_replication_exception(self, response):
        if response.status_code != 500:
            return False
        try:
            exception = response.json()
        except ValueError:
            return False
        return (exception.get('RemoteException', {}).get('exception') ==
                'ArrayIndexOutOfBoundsException')


def webhdfs_client_from_config(config):
    routes = Routes(config.api_url)
    if not routes.api_version in SUPPORTED_VERSIONS:
        versions = ", ".join([str(version) for version in SUPPORTED_VERSIONS])
        raise ExitWithError(
            -1, "API version %s is unsupported. Supported versions: %s" %
            (routes.api_version, versions))
    response = requests.get(routes.storage, auth=config.credentials)
    if response.status_code != 200:
        raise ResponseError("Cannot get WebHDFS details", response)
    details = response.json()
    log.info("Webhdfs at %s for user %s" % (details["location"],
                                            details["user"]))
    return WebHdfsClient(details["location"], details["user"])


@c.with_config
def put_file(args, config):
    """Upload a file to HDFS"""
    client = webhdfs_client_from_config(config)
    if args.remote_path.endswith('/'):
        target_path = args.remote_path
    else:
        remote_type = client.path_type(args.remote_path)
        if remote_type == 'DIRECTORY':
            target_path = os.path.join(args.remote_path,
                                       os.path.split(args.local_file.name)[-1])
            log.info("Path %s is an existing directory, uploading to %s" %
                     (args.remote_path, target_path))
        elif remote_type == 'FILE':
            raise ExitWithError(-1, "Path %s already exists" % args.remote_path)
        else:
            target_path = args.remote_path
    client.put_file(args.local_file, target_path)
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
    client = webhdfs_client_from_config(config)
    statuses = client.list_path(args.path)
    if statuses is None:
        raise ExitWithError(404, "Directory %s not found" % args.path)
    for line in format_statuses(statuses):
        print line.rstrip()
    if not statuses:
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
    client = webhdfs_client_from_config(config)
    with open(target_path, "wb") as out_file:
        size = client.get_file(args.remote_path, out_file)
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
    client = webhdfs_client_from_config(config)
    deleted = client.delete_path(args.path, args.recursive)
    print "%s was %s deleted" % (args.path,
                                 "successfully" if deleted else "not")


def add_rm_command(subparsers):
    get_parser = subparsers.add_parser(
        'rm', help='delete a file or directory in the persistent storage')
    get_parser.add_argument('--recursive', '-r', action='store_true',
                            help='recursive delete')
    get_parser.add_argument('path', help='remote path to delete')
    get_parser.set_defaults(func=rm_command)

def add_commands(subparsers):
    add_put_command(subparsers)
    add_ls_command(subparsers)
    add_get_command(subparsers)
    add_rm_command(subparsers)

