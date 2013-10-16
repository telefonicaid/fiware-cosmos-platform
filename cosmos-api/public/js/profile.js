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
        credentials.slideToggle();
        return false;
    };

    showLink.click(toggleVisibility);
    hideLink.click(toggleVisibility);

    $('.key').each(function(idx, keyNode) {
        var signatureDiv = $(keyNode).find('.signature');
        var textarea = $(keyNode).find('textarea');
        var errorBox = $(keyNode).find('.edit-key-error');

        var setNoEditable = function() {
            $(keyNode).removeClass('edit');
            $(keyNode).addClass('noedit');
        };

        // Switch to key edition
        $(keyNode).find('.edit-key').click(function(ev) {
            ev.preventDefault();
            $(keyNode).removeClass('noedit');
            $(keyNode).addClass('edit');
            textarea.val(signatureDiv.text());
            textarea.select().focus();
            errorBox.hide();
        });

        // Cancel key edition
        $(keyNode).find('.cancel-edit').click(function(ev) {
            ev.preventDefault();
            setNoEditable();
        });

        // Submit PUT
        $(keyNode).find('form').submit(function(ev) {
            ev.preventDefault();
            var newSignature = textarea.val();
            var data = {
                handle: $('#handle').text(),
                keys: [{
                    name: 'default',
                    signature: newSignature
                }]
            };
            var submitButton = $(keyNode).find(".submit");
            submitButton.attr("disabled", "disabled");
            $.ajax({
                url: '/cosmos/v1/profile',
                type: 'PUT',
                contentType: 'json',
                dataType: 'json',
                data: JSON.stringify(data),
                success: function(data, textStatus, xhr) {
                    signatureDiv.text(newSignature);
                    setNoEditable();
                },
                error: function(xhr, textStatus, errorThrown) {
                    var response = JSON.parse(xhr.responseText);
                    errorBox.text(response.message ||
                        response["/keys(0)/signature"] ||
                        "Rejected by the server");
                    errorBox.show();
                },
                complete: function() {
                    submitButton.removeAttr("disabled");
                }
            })
        });
    })
});
