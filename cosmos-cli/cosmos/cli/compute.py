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
import json

import cosmos.cli.command_util as util
from cosmos.cli.config import load_config
from cosmos.cli.tables import format_table
from cosmos.compute.protocol import Protocol


class ComputeCommand(object):
    """Template class for compute commands. It takes care of configuration
    loading and protocol initialization.
    """

    def __init__(self, command):
        self.__command = command

    def __call__(self, args):
        """Callable interface of this command"""
        config = load_config(args)
        proto = Protocol(config.api_url, (config.api_key, config.api_secret))
        return self.__command(args, proto)


def print_clusters(clusters):
    NAME_MAX_WIDTH = 20
    console_width = util.get_console_width()

    print "Available clusters:"
    table = [[util.ellipsize(c["name"], NAME_MAX_WIDTH), c["id"],
              c["state"], c["creationDate"]] for c in clusters]
    for line in format_table(table, 'rlll', separator="  "):
        print util.ellipsize(line, console_width)


def list_clusters(args, proto):
    body = proto.get_clusters()
    clusters = body["clusters"]
    if not clusters:
        print "No available clusters"
    else:
        print_clusters(clusters)
    return 0


def add_list_services_command(subparsers):
    parser = subparsers.add_parser("list-services",
        help="list optional services that can be installed in clusters")
    parser.set_defaults(func=ComputeCommand(list_services))


def list_services(args, proto):
    services = proto.list_services()
    if not services:
        print "No optional services"
    else:
        print_services(services)
    return 0


def print_services(services):
    print "Optional services:"
    for service in services:
        print "\t%s" % service


def add_list_clusters_command(subparsers):
    parser = subparsers.add_parser("list", help="list clusters")
    parser.set_defaults(func=ComputeCommand(list_clusters))


def show_cluster(args, proto):
    EXCLUDED_CLUSTER_INFO = ['href']
    body = proto.get_cluster_details(args.cluster_id)

    print json.dumps(
        util.filtered(body, EXCLUDED_CLUSTER_INFO), sort_keys=True, indent=4)
    return 0


def add_show_cluster_command(subparsers):
    parser = subparsers.add_parser("show", help="show cluster details")
    util.add_cluster_id_argument(parser)
    parser.set_defaults(func=ComputeCommand(show_cluster))


def create_cluster(args, proto):
    services = proto.list_services()
    arg_services_set = set(args.services) if args.services else set()
    unknown_services = arg_services_set - set(services)
    if unknown_services:
        print "Some of the requested services are not recognized:"
        for service in unknown_services:
            print "\t" + service
        return 1
    body = proto.create_cluster(args.name, args.size, list(arg_services_set))
    print "Provisioning new cluster %s" % body["id"]
    return 0


def add_create_cluster_command(subparsers):
    parser = subparsers.add_parser("create", help="create a new cluster")
    parser.add_argument("--name", required=True, help="cluster name")
    parser.add_argument("--size", required=True, type=util.at_least_2,
                        help="number of machines (at least 2")
    parser.add_argument("--services", required=False, nargs="*",
                        help='optional services to install in the cluster')
    parser.set_defaults(func=ComputeCommand(create_cluster))


def terminate_cluster(args, proto):
    body = proto.terminate_cluster(args.cluster_id)
    print "Terminating cluster %s" % args.cluster_id


def add_terminate_cluster_command(subparsers):
    parser = subparsers.add_parser("terminate", help="terminate cluster")
    util.add_cluster_id_argument(parser)
    parser.set_defaults(func=ComputeCommand(terminate_cluster))


def add_compute_commands(subparsers):
    add_list_clusters_command(subparsers)
    add_list_services_command(subparsers)
    add_show_cluster_command(subparsers)
    add_create_cluster_command(subparsers)
    add_terminate_cluster_command(subparsers)

