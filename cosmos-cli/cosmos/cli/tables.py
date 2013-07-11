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
"""Module to format tables represented as lists of lists of strings."""


class Alignment:
    """Column alignments"""
    LEFT = "l"
    RIGHT = "r"


def min_column_widths(rows):
    """Computes the minimum column width for the table of strings.

    >>> min_column_widths([["some", "fields"], ["other", "line"]])
    [5, 6]
    """
    def lengths(row): return map(len, row)
    def maximums(row1, row2) : return map(max, row1, row2)
    return reduce(maximums, map(lengths, rows))


def pad(field, width, alignment):
    """Pad a field to a width aligning it to the right or to the left.

    Use L for left alignment:
    >>> pad("text", 8, Alignment.LEFT)
    'text    '

    And R for right alignment:
    >>> pad("text", 8, Alignment.RIGHT)
    '    text'

    Doesn't work for unknown alignments:
    >>> pad("text", 8, "c")
    Traceback (most recent call last):
        ...
    ValueError: Unknown alignment 'c'
    """
    if alignment == Alignment.LEFT:
        return field.ljust(width)
    elif alignment == Alignment.RIGHT:
        return field.rjust(width)
    else:
        raise ValueError("Unknown alignment '%s'" % alignment)


def format_table(rows, alignments, separator=" "):
    """Formats a table (list of lists) of strings by padding and aligning
    each column.

    This functions is a generator so use list or similar to view the
    results on the REPL:
    >>> list(format_table([["1", "user1", "file.txt"], \
                           ["100", "nobody", ".git"]], "rll"))
    ['  1 user1  file.txt', '100 nobody .git    ']

    >>> list(format_table([], 'rll'))
    []
    """
    if not rows:
        return
    column_widths = min_column_widths(rows)
    for row in rows:
        yield separator.join([pad(field, width, alignment)
                              for (field, width, alignment)
                              in zip(row, column_widths, alignments)])
