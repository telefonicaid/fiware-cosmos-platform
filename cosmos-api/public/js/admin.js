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

;"use strict";

function configureActiveClustersSection() {
    $('#active-clusters').dataTable({
        "sScrollX": "100%",
        "bProcessing": true,
        "sAjaxSource": '/cosmos/v1/stats/clusters',
        "sAjaxDataProp": "clusters",
        "aoColumns": [
            { "mData": "id", "sClass": "id-cell" },
            { "mData": "name" },
            { "mData": "ownerHandle" },
            { "mData": "size", "sType": "numeric", "sClass": "numeric-cell" },
            { "mData": "master" }
        ]
    });
    $.ajax({
        url: '/cosmos/v1/stats/machines',
        type: 'GET',
        contentType: 'json',
        success: function(data) {
            $('#available-machines').text(data['g1-compute'].available)
        }
    });
}

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
    configureActiveClustersSection();
    configureMaintenanceButtons();
});
