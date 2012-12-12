#!/bin/env python
# (c) Copyright 2012 Telefonica, I+D. Printed in Spain (Europe). All Rights
# Reserved.
#
# The copyright to the software program(s) is property of Telefonica I+D.
# The program(s) may be used and or copied only with the express written
# consent of Telefonica I+D or in accordance with the terms and conditions
# stipulated in the agreement/contract under which the program(s) have
# been supplied.

from optparse import OptionParser
import re
import os
import sys


def validation_error(input_line, ref_line):
    print "VALIDATION ERROR: input line:"
    print "   " + input_line
    print "does not match ref line:"
    print "   " + ref_line
    exit(1)


def diff_files(input_file, ref_file):

    input = open(input_file, 'r')
    ref = open(ref_file, 'r')

    input_lines = input.readlines()

    for ref_line in ref.readlines():
        # Get line from input file
        try:
            input_line = input_lines.pop(0)
        except IndexError:
            print "VALIDATION ERROR: input file has less lines than reference"
            exit(1)

        # Removing trailing whitespace(to avoid "noisy" input/reference files)
        ref_line = ref_line.rstrip()
        input_line = input_line.rstrip()

        # Check if normal line or regex(using regex itself
        m = re.match('(.*)REGEX\((.*)\)(.*)', ref_line)
        if m is not  None:
            # We build the regex, concatenating preamble,
            # regex expression itself and the last part
            regex = m.group(1) + m.group(2) + m.group(3)

            if not re.match(regex, input_line):
                validation_error(input_line, ref_line)
        else:
            if not ref_line == input_line:
                validation_error(input_line, ref_line)

    print "Validation ok"
    exit(0)


def main():
    parser = OptionParser()
    parser.add_option("-i", "--input", dest="input_file",
                      help="Input file to be tested")
    parser.add_option("-r", "--reference", dest="ref_file",
                      help="Reference file to be used as a comparison")
    (options, args) = parser.parse_args()

    if not options.input_file:
        parser.error("Missing input file")
    else:
        if not os.path.exists(options.input_file):
            parser.error("Input file %s does not exist" % options.input_file)

    if not options.ref_file:
        parser.error("Missing test file")
    else:
        if not os.path.exists(options.ref_file):
            parser.error("Reference file %s does not exist" % options.ref_file)

    diff_files(options.input_file, options.ref_file)


if __name__ == "__main__":
    main()
