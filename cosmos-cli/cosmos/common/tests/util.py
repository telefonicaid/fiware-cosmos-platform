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
"""Reused test utilities like mocks"""


import mock


def mock_response(status_code=200, json=None, raw=None, text=None):
    response = mock.MagicMock()
    response.status_code = status_code
    if json is None:
        response.json.side_effect = ValueError("Not JSON")
    elif isinstance(json, mock.Mock):
        response.json = json
    else:
        response.json.return_value = json
    if raw is not None:
        response.raw = raw
    if text is not None:
        response.text = text
    return response

