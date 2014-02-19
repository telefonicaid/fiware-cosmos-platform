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
"""General info command"""

import json

from cosmos.cli.config import with_config
from cosmos.common.cosmos_requests import CosmosRequests
from cosmos.common.exceptions import ResponseError
from cosmos.common.routes import Routes


def info_command(args, config):
    response = CosmosRequests(config.credentials).get(Routes(config.api_url).info)
    if response.status_code != 200:
        raise ResponseError("Cannot get general information", response)
    print json.dumps(response.json(), sort_keys=True, indent=4)
    return 0


def add_info_command(subcommands):
    parser = subcommands.add_parser(
        "info", help="Show general information about the platform")
    parser.set_defaults(func=with_config(info_command))
