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
"""SSH connection with clusters"""


import logging as log
import subprocess
import time
from os import path

import cosmos.cli.command_util as util
import cosmos.cli.config as c
from cosmos.cli.util import ExitWithError
from cosmos.common.cosmos_requests import CosmosRequests
from cosmos.common.exceptions import ResponseError
from cosmos.common.routes import Routes


RETRY_WAIT = 10


def ssh_cluster(cluster_id, config):
    """Try to connect to a cluster with SSH"""
    util.set_last_cluster_id(cluster_id)
    cluster = wait_for_cluster_master(cluster_id, config)
    try:
        address = cluster['master']['ipAddress']
    except KeyError:
        raise ExitWithError(-1, 'Unknown master node for %s' % cluster_id)
    command_line = [config.ssh_command, address,
                    '-l', get_user_handle(config),
                    '-o', 'UserKnownHostsFile=/dev/null',
                    '-o', 'StrictHostKeyChecking=no']
    if config.ssh_key is not None and config.ssh_key:
        command_line.extend(['-i', path.expanduser(config.ssh_key)])
    log.info('SSH command: ' + ' '.join(command_line))
    try:
        return subprocess.call(command_line)
    except OSError:
        raise ExitWithError(
            -1, ("Cannot execute '%s' command.\n" +
                 "Check that it is installed in your system and available on " +
                 "your path or change the\nssh_command setting.") %
            config.ssh_command)


def wait_for_cluster_master(cluster_id, config):
    """Poll the cluster resource until master node is ready or raise exception
    if it no longer possible. Returns the last received cluster description."""
    while True:
        cluster = get_cluster_details(cluster_id, config)
        state = cluster['state']
        if state in ['terminating', 'terminated', 'failed']:
            raise ExitWithError(-1, 'Cannot connect to a cluster in %s state' %
                                state)
        if state == 'running' and cluster.has_key('master'):
            return cluster
        print 'Cluster in %s state, waiting for it...' % state
        time.sleep(RETRY_WAIT)


def get_cluster_details(cluster_id, config):
    """Gets the JSON cluster description or raises an exception"""
    response = CosmosRequests(config.credentials).get(
            Routes(config.api_url).cluster(cluster_id))
    if response.status_code == 404:
        raise ExitWithError(404, "Cluster %s does not exist" % cluster_id)
    if response.status_code != 200:
        raise ResponseError("Cannot get cluster details for %s" % cluster_id,
                            response)
    return response.json()


def get_user_handle(config):
    """Looks up the user handle or raises exception"""
    response = CosmosRequests(config.credentials).get(
            Routes(config.api_url).profile)
    if response.status_code != 200:
        raise ResponseError('Cannot access user profile details', response)
    return response.json()['handle']


def add_ssh_command(subcommands):

    @c.with_config
    def command(args, config):
        return ssh_cluster(args.cluster_id, config)

    parser = subcommands.add_parser(
        "ssh", help="Start a SSH connection with the cluster")
    util.add_cluster_id_argument(parser)
    parser.set_defaults(func=command)

