#!/usr/bin/env python

import sys
from mako.template import Template
from subprocess import PIPE, Popen

def exec_template(filename):
   template = Template(filename=filename)
   config = template.render(
      srv_master_host='localhost',
      srv_master_port='35873',
      bridge_host='localhost',
      bridge_port='35800',
      client_master_host='localhost',
      client_master_port='35873')
      
   shell = Popen(['flume', 'shell', '-q'], stdin=PIPE)
   shell.stdin.write(config)
   shell.stdin.close()
   shell.wait()

if len(sys.argv) < 2:
   print("""error: invalid argument count
usage: %s <template-0> [<template-1> ... <template-n>] 
""" % sys.argv[0])
   sys.exit(-1)

for fname in sys.argv[1:]:
   exec_template(fname)