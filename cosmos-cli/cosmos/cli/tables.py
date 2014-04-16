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
