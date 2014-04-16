/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
