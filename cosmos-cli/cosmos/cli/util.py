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


class ExitWithError(Exception):
    """To be thrown on a fatal condition. With exit_code you can control the
    command exit code."""

    def __init__(self, exit_code, explanation):
        super(ExitWithError, self).__init__(explanation)
        self.exit_code = exit_code
        self.explanation = explanation
