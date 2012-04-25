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

var Cosmos = new Class({

    Extends: Hue.JBrowser,

    options: {
        className: 'art browser logo_header cosmos'
    },

    initialize: function(path, options){
        this.parent(path || '/cosmos/', options);
        this.addEvent('load', this.setup.bind(this));
    },

    setup: function(view) {
        (function() {
            // delay to avoid "obj.CallFunction is not a function" error
            // when communicating with the flash object
            var uploaderNode = $(this).getElement('.cos-upload');
            if (uploaderNode) {
                this.upgrade_to_flash(uploaderNode);
            }
        }).delay(10, this);
    },

    upgrade_to_flash: function(uploaderNode) {
        uploaderNode.target = '';
        uploaderNode.href += '?flash_upload=1';
        uploaderNode.addEvent('click', this.make_uploader.bind(this));
    },

    make_uploader: function() {
        (function() {
            var link = $('select-file')
            var linkIdle = link.get('html')

            function linkUpdate() {
                if (!swf.uploading) return;
                var size = link.set('html', '<span class="small">' +
                swf.percentLoaded + '% of ' + size + '</span>');
            }

            var swf = new Swiff.Uploader({
                path: '/static/js/ThirdParty/digitarald-fancyupload/source/Swiff.Uploader.swf',
                url: '/cosmos/flash_upload/',
                verbose: true,
                queued: false,
                multiple: false,
                target: link,
                instantStart: true,
                fileSizeMax: 512 * 1024 * 1024,
                onSelectSuccess: function(files) {
                    this.setEnabled(false);
                },
                onSelectFail: function(files) {
                    alert("No files");
                },
                appendCookieData: true,
                onQueue: linkUpdate,
                onFileComplete: function(file) {
                    if (file.response.error) {
                        alert("Upload of " + file + " failed");
                    } else {
                        var hdfs_path = JSON.decode(file.response.text, true).hdfs_path;
                        alert("Successful upload", "Uploaded to " + hdfs_path);
                    }
                    file.remove();
                    this.setEnabled(true);
                },
                onComplete: function() {
                    link.set('html', linkIdle);
                }
            });

            link.addEvents({
                click: function() {
                    return false;
                },
                mouseenter: function() {
                    this.addClass('hover');
                    swf.reposition();
                },
                mouseleave: function() {
                    this.removeClass('hover');
                    this.blur();
                },
                mousedown: function() {
                    this.focus();
                }
            });
        }).delay(100);
    }
});
