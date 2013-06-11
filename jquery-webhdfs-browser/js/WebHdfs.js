/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
;define(['jquery'], function($) {
    'use strict';
    return function(baseUrl, user) {
        return {
            getHomeDir: function() {
                return $.getJSON(baseUrl + '?user=' + user + '&op=GETHOMEDIRECTORY')
                    .then(function(data) {
                        return data.Path;
                    });
            },
            listDir: function(path) {
                return $.getJSON(baseUrl + path + '/?user=' + user + '&op=LISTSTATUS')
                    .then(function(data) {
                        return data.FileStatuses.FileStatus;
                    });
            },
            getFile: function(path) {
                return $.getJSON(baseUrl + path + '/?user=' + user + '&op=OPEN')
                    .then(function(data, textStatus, request) {
                        if (request.statusCode() === '307') {
                            window.location = request.getResponseHeader('Location');
                        } else {
                            throw 'Expecting redirect from WebHDFS, but got status code ' + request.statusCode();
                        }
                    });
            }
        };
    };
});
