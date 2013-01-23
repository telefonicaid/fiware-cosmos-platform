# -*- coding: utf-8 -*-
"""
Java properties file parser.
"""
import re

ESCAPE = u'\\'
KEY_VALUE_SEPARATORS = [u'=', u':']
LINE_TERMINATORS = [u'\n', u'\r']
LINE_COMMENT_MARKS = [u'#', u'!']
ESCAPE_SEQUENCES = {
    u't': u'\t',
    u'f': u'\f',
    u'n': u'\n',
    u'r': u'\r',
    u'"': u'"',
    u"'": u"'",
    u'=': u'=',
    u':': u':',
    ESCAPE: ESCAPE
}


class InvalidPropertiesError(Exception):
    """
    Raised whenever the properties file is not valid.
    """
    pass


def loads(string):
    """
    Loads a property file from its string representation.
    """
    properties = []
    for line in logical_lines(string):
        stripped_line = line.strip()
        if len(stripped_line) == 0 or stripped_line[0] in LINE_COMMENT_MARKS:
            continue # ignored line
        properties.append(parse_pair(line))
    return properties


def logical_lines(string):
    """
    Returns the list of lines that are not blank nor comment-lines taking care
    of line character escapes (backslashes).
    """
    remaining = string
    line = u''

    while len(remaining) > 0:
        current, remaining = popleft(remaining)
        if current == '\\':
            if len(remaining) > 0:
                current, remaining = popleft(remaining)
                if current in LINE_TERMINATORS:
                    remaining = remaining.lstrip()
                else:
                    line += ESCAPE + current
        elif current in LINE_TERMINATORS:
            yield line
            line = u''
        else:
            line += current
    if len(line) > 0:
        yield line


def parse_pair(line):
    """
    Extracks key and value given a properties line.
    """
    sep_index = first_unescaped(line, KEY_VALUE_SEPARATORS)
    if sep_index is None:
        return (unescape(line.strip()), u'')
    else:
        return (unescape(line[0 : sep_index].strip()),
                unescape(line[sep_index + 1:].strip()))


def first_unescaped(text, values):
    """
    Finds the first unescaped appearance of any of values in a text.
    """
    escaped = False
    for index, char in enumerate(text):
        if escaped:
            escaped = False
        elif char == ESCAPE:
            escaped = True
        elif char in values:
            return index
    return None


def unescape(text):
    """
    Replace character escapes by its represented characters.
    """
    remaining = text
    unescaped = u''
    while len(remaining) > 0:
        current, remaining = popleft(remaining)
        if current == ESCAPE:
            if remaining.startswith(u'u'):
                uni_escape, remaining = popleft(remaining, count=5)
                unescaped += unicode_char(uni_escape)
            else:
                current, remaining = popleft(remaining)
                unescaped += unescape_char(current)
        else:
            unescaped += current

    return unescaped


def unicode_char(uni_escape):
    try:
        return unichr(int(uni_escape[1:], 16))
    except ValueError:
        raise InvalidPropertiesError("Invalid unicode escaping \\%s" %
                                     uni_escape)


def unescape_char(char):
    if ESCAPE_SEQUENCES.has_key(char):
        return ESCAPE_SEQUENCES[char]
    else:
        raise InvalidPropertiesError("Wrong escape sequence \\%s" % char)


def popleft(string, count=1):
    """
    Pops characters from the beginning of the string and returns both the
    popped ones and the body.
    """
    return string[0:count], string[count:]
