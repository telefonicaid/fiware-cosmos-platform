# -*- coding: utf-8 -*-
#
# Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
"""Simple Cosmos API CLI client"""

import argparse
import logging as log
import sys

from requests.exceptions import ConnectionError, Timeout

import cosmos.cli.config as c
from cosmos.cli.compute import add_compute_commands
from cosmos.cli.info import add_info_command
from cosmos.cli.ssh import add_ssh_command
from cosmos.cli.storage import add_storage_commands
from cosmos.cli.util import ExitWithError
from cosmos.common.exceptions import CosmosException
from cosmos.common.version import print_versions


def add_version_command(subcommands):

    def command(args):
        print_versions()
        return 0

    parser = subcommands.add_parser("version", help="Show version information")
    parser.set_defaults(func=command)


def add_configure_command(subcommands):
    parser = subcommands.add_parser(
        "configure", help="create " + c.default_config_path())
    parser.set_defaults(func=c.command)


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
    add_version_command(subparsers)
    add_info_command(subparsers)
    add_configure_command(subparsers)
    add_ssh_command(subparsers)
    add_compute_commands(subparsers)
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

