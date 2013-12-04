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

;"use strict";

/**
 * Configure AJAX buttons for entering/leaving maintenance mode with user confirmation.
 */
function configureMaintenanceButtons() {
    function configureMaintenanceButton(button, actionDescription, desiredMaintenanceStatus) {
        button.click(function (ev) {
            ev.preventDefault();
            var confirmation = confirm(actionDescription + " will affect all Cosmos Platform users.");
            if (!confirmation) {
                return;
            }
            button.attr("disabled", "disabled");
            $.ajax({
                url: '/cosmos/v1/maintenance',
                type: 'PUT',
                contentType: 'json',
                dataType: 'json',
                data: JSON.stringify(desiredMaintenanceStatus),
                success: function() { location.reload(); },
                error: function(xhr) {
                    $("#maintenance-error").text("Error " + xhr.status);
                    button.removeAttr("disabled");
                }
            });
        });
    }

    configureMaintenanceButton($("#enter-maintenance"), "Entering maintenance mode", true);
    configureMaintenanceButton($("#leave-maintenance"), "Leaving maintenance mode", false);
}

$(document).ready(function() {
    configureMaintenanceButtons();
});