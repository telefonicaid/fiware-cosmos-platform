# -*- coding: utf-8 -*-
"""
IPTables Fabric utilities.
"""
from fabric.api import run, sudo, settings
from fabric.colors import green, white
import fabric.context_managers as ctx


def add_rule(rule):
    """
    Adds a rule to iptables as the first rule only if the rule didn't
    previously exist.

    The string that should be passed to this method is the following:
    Type "iptables -I [your rule here]" on the console. Then type "iptables -S"
    and look for your rule. Everything after the "-I" is the string that should
    be passed to this method.
    """
    with ctx.hide('stdout'):
        # Need to consider this settings context and warn_only
        # to have full control over the run commands.
        with settings(warn_only=True):
            output = run("iptables -S | grep -q -e '%s'" % rule)
            if output.return_code != 0:
                print white("Rule '%s' does not exist. Adding..." % rule, True)
                sudo('iptables -I %s' % rule)
            else:
                print white("Rule '%s' already exists. Will not add again." %\
                            rule, True)
                return 1


def accept_in_tcp(port):
    """
    Adds a rule to iptables for accepting incoming TCP connections to a given
    port.
    """
    add_rule('INPUT -p tcp -m tcp --dport %d -j ACCEPT' % port)


def accept_out_tcp(port):
    """
    Adds a rule to iptables for accepting out TCP connections to a given port.
    """
    add_rule('OUTPUT -p tcp -m tcp --dport %d -j ACCEPT' % port)
