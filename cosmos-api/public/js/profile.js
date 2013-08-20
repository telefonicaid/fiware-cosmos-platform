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

;$(document).ready(function() {
    "use strict";

    var showLink = $("#show-credentials");
    var hideLink = $("#hide-credentials");
    var credentials = $("#credentials");
    var toggleVisibility = function() {
        showLink.toggle();
        hideLink.toggle();
        credentials.toggle(400);
        return false;
    };

    showLink.click(toggleVisibility);
    hideLink.click(toggleVisibility);
});
