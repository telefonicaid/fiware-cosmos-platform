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
"""Configuration handling"""

import sys

from pymlconf import ConfigManager
import yaml

from cosmos.cli.util import ExitWithError
import cosmos.cli.home_dir as home_dir


CONFIG_SETTINGS = [dict(
    key='api_url',
    description='Base API URL',
    default='https://cosmos.hi.inet/cosmos/v1'
), dict(
    key='api_key',
    description='API key',
), dict(
    key='api_secret',
    description='API secret',
), dict(
    key='ssh_command',
    description='SSH command',
    default='ssh'
), dict(
    key='ssh_key',
    description='SSH identity key (empty for the default one)',
    default=''
)]
DEFAULT_CONFIG = dict([(setting['key'], setting['default'])
                       for setting in CONFIG_SETTINGS if 'default' in setting])


def default_config_path():
    """Get a correct configuration path, regardless if we are on a POSIX system
    or a Windows system.

    >>> "cosmosrc" in default_config_path()
    True
    """
    return home_dir.get().get_default_config_filename()


def load_config(args):
    """Tries to load the configuration file or throws ExitWithError."""
    config = ConfigManager(DEFAULT_CONFIG)
    config_dir = home_dir.get()
    try:
        config_contents = config_dir.read_config_file(
            filename_override=args.config_file)
        config.merge(ConfigManager(config_contents))
    except Exception as ex:
        print "Error reading configuration: %s" % ex.message
    if not 'api_key' in config.keys() or not 'api_secret' in config.keys():
        raise ExitWithError(
            -1, ("Cosmos command is unconfigured. Use '%s configure' to " +
                 "create a valid configuration or use --config-file with a " +
                 "valid configuration file") % sys.argv[0])
    config.credentials = (config.api_key, config.api_secret)
    strip_config(config)
    return config


def strip_config(config):
    """Strips all strings in a config"""
    for key in dir(config):
        value = getattr(config, key)
        if type(value) == str:
            setattr(config, key, value.strip())


def with_config(command):
    """Decorates a command to append the configuration as an extra argument.
    This is intended to be use as a decorator.
    """

    def decorated_command(*args, **kwargs):
        extended_args = list(args)
        extended_args.append(load_config(args[0]))
        return command(*extended_args, **kwargs)

    return decorated_command


def ask_for_setting(config, setting):
    """Interactively ask for a setting using current value as a default."""
    default = config.get(setting['key'], "")
    answer = raw_input("%s [%s]: " % (setting['description'], default)).strip()
    if not answer:
        answer = default
    config[setting['key']] = answer


def command(args):
    """Create a configuration file by asking for the settings"""
    config = ConfigManager(DEFAULT_CONFIG)
    for setting_dict in CONFIG_SETTINGS:
        ask_for_setting(config, setting_dict)
    contents = yaml.dump(dict(config.items()), default_flow_style=False)
    home_dir.get().write_config_file(contents)
    return 0
