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
