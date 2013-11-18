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
 * Make credentials visibility toggle to avoid casually revealing user
 * credentials.
 *
 * Credentials div should be initially hidden.
 */
function configureCredentialsToggle() {
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
}

/**
 * Post a profile update
 *
 * @param data        Complete profile to update
 * @param onSuccess   Success handler
 * @param onError     Error handler
 * @param onComplete  Completion handler
 */
function updateProfile(data, onSuccess, onError, onComplete) {
    $.ajax({
        url: '/cosmos/v1/profile',
        type: 'PUT',
        contentType: 'json',
        dataType: 'json',
        data: JSON.stringify(data),
        success: onSuccess,
        error: onError,
        complete: onComplete
    })
}

/**
 * Extracts current profile values from the fields shown to the user.
 * @returns Profile as expected by the profile resource
 */
function currentProfile() {
    return {
        handle: $('#handle').text(),
        email: $('#email').text(),
        keys: [{
            name: 'default',
            signature: $('.signature').first().text()
        }]
    };
}

/**
 * Make email editable and, when OAuth profile email and current
 * email are different allow for synchronizing them.
 */
function configureEmailEditing() {
    var editionArea = $('#edit-email-area');
    var emailInput = $('#new-email');
    var emailNode = $('#email');
    var editLink = $('#edit-email');
    var submitButton = editionArea.find('.submit');
    var errorBox = $('#email-error');
    var syncLink = $('#sync-email');

    editLink.click(function startEditing(ev) {
        ev.preventDefault();
        editionArea.show();
        editLink.hide();
        syncLink.hide();
        emailInput.val(emailNode.text());
        emailInput.select().focus();
    });

    function stopEditing() {
        editionArea.hide();
        editLink.show();
        errorBox.hide();
        emailNode.change();
    }

    editionArea.find('.cancel-edit').click(function (ev) {
        ev.preventDefault();
        stopEditing();
    });

    submitButton.click(function (ev) {
        ev.preventDefault();
        submitButton.attr("disabled", "disabled");
        var data = currentProfile();
        var newEmail = emailInput.val();
        data.email = newEmail;
        updateProfile(data, function onSuccess() {
            emailNode.text(newEmail);
            emailNode.change();
            stopEditing();
        }, function onError(xhr) {
            var response = JSON.parse(xhr.responseText);
            errorBox.text(response.message ||
                response["/email"] ||
                "Rejected by the server");
            errorBox.show();
        }, function onComplete() {
            submitButton.removeAttr("disabled");
        });
    });

    // Reset email to profile value
    var oauthEmail = syncLink[0].getAttribute('data-oauth-email');
    function updateSyncVisibility() {
        if (emailNode.text() === oauthEmail) {
            syncLink.hide();
        } else {
            syncLink.show()
        }
    }
    updateSyncVisibility();
    emailNode.change(updateSyncVisibility);
    syncLink.click(function (ev) {
        ev.preventDefault();
        var data = currentProfile();
        data.email = oauthEmail;
        updateProfile(data, function onSuccess() {
            emailNode.text(oauthEmail);
            emailNode.change();
        }, function onError(xhr) {
            var response = JSON.parse(xhr.responseText);
            var cause = response.message || response["/email"] ||
                "update rejected";
            alert("Update error: " + cause);
        });
    });
}

/**
 * Configure key number idx to be editable by the user.
 *
 * @param idx      Index
 * @param keyNode  Node of the key
 */
function configureKeyEditing(idx, keyNode) {
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
        var data = currentProfile();
        var newSignature = textarea.val();
        data.keys[0].signature = newSignature;
        var submitButton = $(keyNode).find(".submit");
        submitButton.attr("disabled", "disabled");
        updateProfile(data, function onSuccess() {
            signatureDiv.text(newSignature);
            setNoEditable();
        }, function onError(xhr) {
            var response = JSON.parse(xhr.responseText);
            errorBox.text(response.message ||
                response["/keys(0)/signature"] ||
                "Rejected by the server");
            errorBox.show();
        }, function onComplete() {
            submitButton.removeAttr("disabled");
        });
    });
}

$(document).ready(function() {
    configureCredentialsToggle();
    configureEmailEditing();
    $('.key').each(configureKeyEditing)
});
