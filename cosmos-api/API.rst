=============================
Cosmos REST API documentation
=============================

----------
Versioning
----------

A version tag is included in every resource address to allow for controlled API
evolution, e.g. ``/cosmos/v1/``.  Current API version tag is ``v1`` and will
remain unchanged for incremental, backward-compatible changes.

Once a breaking change is introduced, version tag will be bumped (``v2`` for
instance). When a request addressed to the older API version is received it is
either:

- redirected with 301 REDIRECT to the new resource when compatible
- returned a 410 GONE status

--------------
Authentication
--------------

Requests should use a basic Authorization header as in RFC 2617 in which
username corresponds with the API key and the password with the API secret.
Alternatively, a session cookie of a valid user is also accepted as valid
authentication to ease API exploration and direct use from JavaScript.

-------------------------
Resources of the user API
-------------------------

GET ``/cosmos/v1``
------------------

*Since v1*

Represents endpoint for the REST API:

    {
      "message": "Endpoint for Cosmos API"
    }

GET ``/cosmos/v1/profile``
--------------------------

*Since v1*

Represents general user profile information as JSON::

    {
      "handle": <string>,
      "keys": [
        { "name": <string>, "signature": <string> },
        { "name": <string>, "signature": <string> }
      }
    }

PUT ``/cosmos/v1/profile``
--------------------------

*Since v1*

Allow for updating user public keys by sending a JSON body::

    {
      "handle": <string>,
      "keys": [
        { "name": <string>, "signature": <string> }
      }
    }

Despite only the key can be changed the handle should be included in the
payload. Note that exactly one public key is required.

In case of success the same body is returned with a 200 OK status. Otherwise,
a 400 status with an error code::

    {
        "message": "<error message>"
    }

GET ``/cosmos/v1/cluster``
--------------------------

*Since v1*

Represents all the user clusters. Listing is of the form::

    {
      "clusters": [
        {
          "id": <string>,
          "href": <url>,
          "name": <string>,
          "state": <string>,
          "stateDescription": <string>,
          "creationDate": <ISO_datetime>
        },
        {
          "id": <string>,
          "href": <url>,
          "name": <string>,
          "state": <string>,
          "stateDescription": <string>,
          "creationDate": <ISO_datetime>
        }
      ]
    }

State is one of the following options: ``provisioning``, ``running``,
``terminating``, ``terminated`` or ``failed``.  This state information is
complemented on the ``stateDescription`` field.

POST ``/cosmos/v1/cluster``
---------------------------

*Since v1*

Ask for a new cluster provision. Request is of the form::

    {
      "name": <string>,
      "size": <int>
    }

In case of success the response will have status 201 Created and a Location
header. Additionally, the body will be as follows::

    {
      "id": <string>,
      "href": <url>
      "name": <string>,
      "state": <string>,
      "stateDescription": <string>
    }

GET  ``/cosmos/v1/cluster/<id>``
--------------------------------

*Since v1*

Consult details of the cluster with id ``<id>``. Body as follows::

    {
      "id": <string>,
      "name": <string>,
      "state": <string>,
      "stateDescription": <string>,
      "href": <string>,
      "size": <int>
    }

State related fields have the same meaning as in ``/cosmos/v1/cluster`` GET
response.

POST ``/cosmos/v1/cluster/<id>/terminate``
------------------------------------------

*Since v1*

Terminates the cluster with id ``<id>``. Returns immediately so check the
``state`` field by means of a GET to check termination status.


GET ``/cosmos/v1/storage``
--------------------------

*Since v1*

Provides the information needed for accessing persistent storage. At the moment
it consists on WebHdfs url and username::

    {
      "location": <string>,
      "user": <string>
    }

--------------------------
Resources of the admin API
--------------------------

POST ``/admin/v1/user``
----------------------

*Since v1*

This resource follows an authentication scheme different for the client API.
Instead of using the pair API id / secret, a different set of credentials are
accepted per authentication realm.  This is configured and enabled on the
`cosmos-api` configuration file.

Provides a mean for user registration by posting the properties of the newly
created user.  The properties have the following restrictions:

 * `authId`: non-empty string that must be unique per authorization realm.
 * `authRealm`: identifier of the authorization realm (also a non-empty string).
 * `handle`: user handle to be used as SSH login. It must be a valid unix login
   (letters and numbers with a leading letter) and at least three characters.
   If this field is not present, one will be generated.
 * `sshPublicKey`: must be a public key in the same format SSH stores it
   (`ssh-rsa|ssh-dsa`, the key and the user email).

Sample body::

    {
      "authId": "id",
      "authRealm": "realm",
      "handle": "handle",
      "sshPublicKey": "ssh-rsa CKDKDJDJD user@host"
    }

In case of success, a 201 status with the following body scheme is returned::

    {
      "handle": "handle",
      "apiKey": "XXXXXXXXX",
      "apiSecret": "YYYYYYYYYYYYYYYYYYYY"
    }

Otherwise, one of the following errors will be returned:

 * Unauthorized 401
 * Forbidden 403
 * Bad request 400, invalid JSON payload.
 * Conflict 409, already existing handle.
 * Conflict 409, already existing credentials.
 * Internal server error 500, account registration failed.

