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
"""Simple Cosmos API CLI client"""

import argparse
import json
import logging as log
import sys

import requests
from requests.exceptions import ConnectionError, Timeout

import cosmos.cli.config as c
from cosmos.cli.command_util import add_cluster_id_argument
from cosmos.cli.ssh import add_ssh_command
from cosmos.cli.storage import add_storage_commands
from cosmos.cli.tables import format_table
from cosmos.cli.util import ExitWithError
from cosmos.common.exceptions import ResponseError
from cosmos.common.routes import Routes


ELLIPSIS = '...'


def add_configure_command(subcommands):
    parser = subcommands.add_parser(
        "configure", help="create " + c.default_config_path())
    parser.set_defaults(func=c.command)


def ellipsize(text, max_width):
    """Truncate long texts with ellipsis.
    >>> ellipsize("Very long text", 10)
    'Very lo...'
    >>> ellipsize("Short", 10)
    'Short'
    >>> ellipsize("Too small width", 2)
    '...'
    """
    if len(text) <= max_width:
        return text
    if len(ELLIPSIS) >= max_width:
        return ELLIPSIS
    return text[: max_width - len(ELLIPSIS)] + ELLIPSIS


def add_list_command(subcommands):

    NAME_MAX_WIDTH = 20
    MAX_WIDTH = 78

    @c.with_config
    def list_clusters(args, config):
        """List existing clusters"""
        r = requests.get(Routes(config.api_url).clusters(),
                         auth=config.credentials)
        if r.status_code != 200:
            raise ResponseError("Cannot list clusters", r)
        clusters = r.json()["clusters"]
        if not clusters:
            print "No available clusters"
        else:
            print "Available clusters:"
            table = [[ellipsize(c["name"], NAME_MAX_WIDTH), c["id"],
                      c["state"], c["stateDescription"]] for c in clusters]
            for line in format_table(table, 'rlll', separator="  "):
                print ellipsize(line, MAX_WIDTH)
        return 0

    parser = subcommands.add_parser("list", help="list clusters")
    parser.set_defaults(func=list_clusters)


def add_show_command(subcommands):

    @c.with_config
    def cluster_details(args, config):
        r = requests.get(Routes(config.api_url).cluster(args.cluster_id),
                         auth=config.credentials)
        if r.status_code != 200:
            raise ResponseError(
                "Cannot get details for %s" % args.cluster_id, r)

        print json.dumps(r.json(), sort_keys=True, indent=4)
        return 0

    parser = subcommands.add_parser("show", help="show cluster details")
    add_cluster_id_argument(parser)
    parser.set_defaults(func=cluster_details)


def at_least_2(argument):
    """Parses integers equal or greater than 2.
    >>> at_least_2('2')
    2
    >>> at_least_2('1')
    Traceback (most recent call last):
        ...
    ArgumentTypeError: 1 is less than 2
    """
    amount = int(argument)
    if amount <= 1:
        raise argparse.ArgumentTypeError("%r is less than 2" % amount)
    return amount


def add_create_command(subcommands):

    @c.with_config
    def create_cluster(args, config):
        """Trigger cluster provisioning"""
        r = requests.post(Routes(config.api_url).clusters(),
                          json.dumps({"name": args.name, "size": args.size}),
                          auth=config.credentials)
        if r.status_code != 201:
            raise ResponseError("Cluster creation problem", r)

        print "Provisioning new cluster %s" % r.json()["id"]
        return 0

    parser = subcommands.add_parser("create", help="create a new cluster")
    parser.add_argument("--name", required=True, help="cluster name")
    parser.add_argument("--size", required=True, type=at_least_2,
                        help="number of machines (at least 2")
    parser.set_defaults(func=create_cluster)


def add_terminate_command(subcommands):

    @c.with_config
    def terminate_cluster(args, config):
        """Trigger cluster termination"""
        r = requests.post(
            Routes(config.api_url).cluster(args.cluster_id, action="terminate"),
            auth=config.credentials)
        if r.status_code != 200:
            raise ResponseError(
                "Cannot terminate cluster %s" % args.cluster_id, r)

        print "Terminating cluster %s" % args.cluster_id

    parser = subcommands.add_parser("terminate", help="terminate cluster")
    add_cluster_id_argument(parser)
    parser.set_defaults(func=terminate_cluster)


def build_argument_parser():
    """Register all subcommands in a unified argument parser."""
    parser = argparse.ArgumentParser(prog='cosmos')
    parser.add_argument('--config-file', '-c',
                        help='Override configuration file')
    parser.add_argument('--verbose', '-v', action='store_true',
                        help='Verbose output')
    subparsers = parser.add_subparsers(help='sub-command help',
                                       title='subcommands',
                                       description='valid subcommands')
    add_configure_command(subparsers)
    add_list_command(subparsers)
    add_show_command(subparsers)
    add_create_command(subparsers)
    add_terminate_command(subparsers)
    add_ssh_command(subparsers)
    add_storage_commands(subparsers)
    return parser


def set_verbose(is_verbose):
    """Configures logging for more or less verbose output."""
    level = log.DEBUG if is_verbose else log.WARNING
    log.basicConfig(format="%(levelname)s: %(message)s", level=level)
    if is_verbose:
        log.info("Verbose output")

def run():
    """Register all subcommands, parse the command line and run the function
    set as default for the parsed command"""
    args = build_argument_parser().parse_args()
    set_verbose(args.verbose)
    exit_code = -1
    try:
        args.func(args)
        exit_code = 0
    except KeyboardInterrupt:
        print "Command cancelled by the user"
    except ExitWithError, ex:
        print ex.explanation
        exit_code = ex.exit_code
    except ConnectionError, ex:
        print "Cannot connect with server:\n%s" % ex.message
    except Timeout, ex:
        print "Request timed out:\n%s" % ex.message
    except CosmosException, ex:
        print str(ex)
    sys.exit(exit_code)


if __name__ == "__main__":
    run()

