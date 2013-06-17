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
import re
import sys

import requests

import cosmos.config as c
import cosmos.webhdfs as webhdfs
from cosmos.routes import Routes
from cosmos.util import ExitWithError, ResponseError


UUID_PATTERN = re.compile(r"^[0-9a-f]{32}$")


def add_configure_command(subcommands):
    parser = subcommands.add_parser(
        "configure", help="create " + c.default_config_path())
    parser.set_defaults(func=c.command)


def add_list_command(subcommands):

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
            for cluster in clusters:
                print " * %s" % cluster["id"]
        return 0

    parser = subcommands.add_parser("list", help="list clusters")
    parser.set_defaults(func=list_clusters)


def cluster_id(argument):
    if not UUID_PATTERN.match(argument):
        raise argparse.ArgumentTypeError("Not a valid cluster id")
    return argument


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
    parser.add_argument("cluster_id", type=cluster_id, help="cluster id")
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
    parser.add_argument("cluster_id", type=cluster_id, help="cluster id")
    parser.set_defaults(func=terminate_cluster)


def build_argument_parser():
    """Register all subcommands in a unified argument parser."""
    parser = argparse.ArgumentParser(prog='cosmos')
    parser.add_argument("--config-file", "-c",
                        help="Override configuration file")
    subparsers = parser.add_subparsers(help='sub-command help',
                                       title='subcommands',
                                       description='valid subcommands')
    add_configure_command(subparsers)
    add_list_command(subparsers)
    add_show_command(subparsers)
    add_create_command(subparsers)
    add_terminate_command(subparsers)
    webhdfs.add_commands(subparsers)
    return parser


def run():
    """Register all subcommands, parse the command line and run the function
    set as default for the parsed command"""
    args = build_argument_parser().parse_args()
    try:
        sys.exit(args.func(args))
    except KeyboardInterrupt:
        print "Command cancelled by the user"
        sys.exit(-1)
    except ExitWithError, ex:
        print ex.explanation
        sys.exit(ex.exit_code)

if __name__ == "__main__":
    run()

