#!/usr/bin/env python

import json
from optparse import OptionParser
from subprocess import PIPE, Popen
import sys

from mako.template import Template

DEFAULT_CONFIG = 'config.json'

def load_config(filename):
    try:
        with open(filename, 'r') as f:
            return json.load(f)
    except Exception, ex:
        print "Cannot read configuration from %s: %s" % (filename, ex)
        sys.exit(-1)


def parse_command_line():
    usage = "Usage: %prog [options] <template-0> [<template-1> ... <template-n>]"
    parser = OptionParser(usage=usage)
    parser.add_option('-c', '--config', dest='config', default=DEFAULT_CONFIG,
                      help='read configuration from FILE', metavar='FILE')
    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.error("invalid argument count")
    return (options, args)


def exec_template(filename, config):
    shell = Popen(['flume', 'shell', '-q'], stdin=PIPE)
    shell.stdin.write(Template(filename=filename).render(config))
    shell.stdin.close()
    shell.wait()


if __name__ == "__main__":
    (options, templates) = parse_command_line()
    config = load_config(options.config)
    for template in templates:
        exec_template(template, config)
