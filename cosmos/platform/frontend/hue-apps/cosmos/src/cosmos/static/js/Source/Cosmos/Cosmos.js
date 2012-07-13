// Licensed to Cloudera, Inc. under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  Cloudera, Inc. licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
/*
---

script: Cosmos.js

description: Defines Cosmos; a Hue application that extends Hue.JBrowser.

authors:
- Sebastian Ortega sortega@tid.es

requires:
- JFrame/JFrame.Browser
- fancyupload/FancyUpload3.Attach

provides: [Cosmos]

...
*/
ART.Sheet.define('window.art.browser.cosmos', {
        'min-width': 620
});

// Utility function. Nice to have: having it available as part of the filters
// framework.
function getUrlParamsMap(url) {
    var mapping = {};
    var queryStringStart = url.indexOf('?');
    if (queryStringStart === -1) {
        return mapping;
    }
    var params = url.slice(queryStringStart + 1).split('&');
    params.forEach(function (param) {
        pair = param.split('=');
        mapping[pair[0]] = pair[1];
    });
    return mapping;
}

var Cosmos = new Class({

    Extends: Hue.JBrowser,

    options: {
        className: 'art browser logo_header cosmos',
        autoreloaded_views: ['index'],
        autoreload_period: 20
    },

    initialize: function(path, options){
        this.parent(path || '/cosmos/', options);
        this.addEvent('load', this.setup.bind(this));
        this.addEvent('load', this.autoreload.bind(this));
        var frame = this.jframe;
        this.jframe.addBehaviors({
            KeySelector: function (element) {
                $(element).addEvent('change', function() {
                    var path = frame.currentPath;
                    var params = getUrlParamsMap(path);
                    params.primary_key = $(element).value;

                    var queryString = '';
                    for (var key in params) {
                        if (queryString) {
                            queryString += '&';
                        } else {
                            queryString += '?';
                        }
                        queryString += escape(key) + '=' + params[key];
                    }
                    var basePath = path.substring(0, path.indexOf('?')) || path;
                    frame.load({requestPath: basePath + queryString});
                });
            }
        });
    },

    setup: function(view) {
        (function() {
            // delay to avoid "obj.CallFunction is not a function" error
            // when communicating with the flash object
            if (Browser.Plugins.Flash.build) {
                this.upgrade_to_flash($(this).getElement('.cos-upload_dataset'));
                this.upgrade_to_flash($(this).getElement('.cos-upload_jar'));
            }
        }).delay(10, this);
    },

    autoreload: function(view) {
        if (this.reload_timeout) {
            clearTimeout(this.reload_timeout);
            delete this.reload_timeout;
        }
        if (this.options.autoreloaded_views.contains(view)) {
            this.reload_timeout = setTimeout(function () {
                this.jframe.refresh();
            }.bind(this), this.options.autoreload_period * 1000);
        }
    },

    upgrade_to_flash: function(uploaderNode) {
        if (!uploaderNode) {
            return;
        }
        uploaderNode.target = '';
        uploaderNode.href += '?flash_upload=1';
        var dest = uploaderNode.get('href').toURI().get('data').dest;

        this.addEvents({
            alert: function(){
                if (this.uploader) this.uploader.uploader.box.hide();
            },
            endAlert: function(){
                if (this.uploader) this.uploader.uploader.box.show();
            }
        });

        this.uploader = new Hue.FileBrowser.Uploader(dest, this.jframe, {
            //the DOM element where we're going to display our results
            list: $(this).getElement('.cos-upload-list'),
            listContainer: $(this).getElement('.cos-uploader'),
            button: uploaderNode,
            uploaderOptions: {
                //call this url when we upload a file
                url: '/filebrowser/upload_flash',
                container: this.toolbar
            }
        });
        
        // Infect Hue.FileBrowser.Uploaded to tune error messages
        var that = {wrappedHandler: this.uploader.uploader.$events.fileError[0]};
        this.uploader.uploader.removeEvent('fileError', that.wrappedHandler);
        this.uploader.uploader.addEvent('fileError', function (file) {
            file.errorMessage = 'Cannot upload file. A file with the same ' +
                                'name might exists';
            this.wrappedHandler(file);
        }.bind(that));
    }
});
