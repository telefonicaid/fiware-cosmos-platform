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

import cosmos.config as config
from cosmos.routes import Routes


UUID_PATTERN = re.compile(r"^[0-9a-f]{32}$")


def print_error_response(description, response):
    print "%s: %d" % (description, response.status_code)
    try:
        error = response.json()["error"]
    except ValueError:
        error = response.text
    print "Error: %s" % error


def add_configure_command(subcommands):
    parser = subcommands.add_parser(
        "configure", help="create " + config.get_config_path())
    parser.set_defaults(func=config.command)


def credentials(config):
    return (config.api_key, config.api_secret)


def add_list_command(subcommands):

    @config.with_config
    def list_clusters(args, config):
        """List existing clusters"""
        r = requests.get(Routes(config.api_url).clusters(),
                         auth=credentials(config))
        if r.status_code != 200:
            print_error_response("Cannot list clusters", r)
            return r.status_code
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

    @config.with_config
    def cluster_details(args, config):
        r = requests.get(Routes(config.api_url).cluster(args.cluster_id),
                         auth=credentials(config))
        if r.status_code != 200:
            print_error_response(
                "Cannot get details for %s" % args.cluster_id, r)
            return r.status_code

        print json.dumps(r.json(), sort_keys=True, indent=4)
        return 0

    parser = subcommands.add_parser("show", help="show cluster details")
    parser.add_argument("cluster_id", type=cluster_id, help="cluster id")
    parser.set_defaults(func=cluster_details)


def add_create_command(subcommands):

    @config.with_config
    def create_cluster(args, config):
        """Trigger cluster provisioning"""
        r = requests.post(Routes(config.api_url).clusters(),
                          json.dumps({"name": args.name, "size": args.size}),
                          auth=credentials(config))
        if r.status_code != 201:
            print_error_response("Cluster creation problem", r)
            return r.status_code

        print "Provisioning new cluster %s" % r.json()["id"]
        return 0

    def atLeast2(argument):
        amount = int(argument)
        if amount <= 1:
            raise argparse.ArgumentTypeError("%r is less than 2" % amount)
        return amount

    parser = subcommands.add_parser("create", help="create a new cluster")
    parser.add_argument("--name", required=True, help="cluster name")
    parser.add_argument("--size", required=True, type=atLeast2,
                        help="number of machines (at least 2")
    parser.set_defaults(func=create_cluster)


def add_terminate_command(subcommands):

    @config.with_config
    def terminate_cluster(args, config):
        """Trigger cluster termination"""
        r = requests.post(
            Routes(config.api_url).cluster(args.cluster_id, action="terminate"),
            auth=credentials(config))
        if r.status_code != 200:
            print_error_response(
                "Cannot terminate cluster %s" % args.cluster_id, r)
            return r.status_code

        print "Terminating cluster %s" % args.cluster_id

    parser = subcommands.add_parser("terminate", help="terminate cluster")
    parser.add_argument("cluster_id", type=cluster_id, help="cluster id")
    parser.set_defaults(func=terminate_cluster)


def run():
    """Register all subcommands, parse the command line and run the function
    set as default for the parsed command"""
    parser = argparse.ArgumentParser(prog='cosmos')
    subparsers = parser.add_subparsers(help='sub-command help',
                                       title="subcommands",
                                       description="valid subcommands")
    add_configure_command(subparsers)
    add_list_command(subparsers)
    add_show_command(subparsers)
    add_create_command(subparsers)
    add_terminate_command(subparsers)

    args = parser.parse_args()
    try:
        sys.exit(args.func(args))
    except KeyboardInterrupt:
        print "Command cancelled by the user"
        sys.exit(-1)

if __name__ == "__main__":
    run()

