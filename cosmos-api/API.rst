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

----------------
Maintenance mode
----------------

When the platform is under maintenance, resources will fail with status 503.

-------------------------
Resources of the user API
-------------------------

Authentication
--------------

Most API requests require authentication so they can be traced to a valid user
in order to be accepted.  There are three supported authentication methods
that can be equally used.

 * **Cosmos API credentials.** Requests should use a basic "Authorization"
   header as in RFC 2617 in which username corresponds with the API key
   and the password with the API secret.  Users can check their credentials
   at their profile pages.  This is the recommended authentication method
   for normal use of the API.

 * **Session cookie.** A session cookie of a valid user is also accepted as
   valid authentication to ease API exploration and direct use from
   JavaScript.  This is recommended when exploring the API from the Swagger
   page.

 * **Authentication token.** Requests should have a ``X-Auth-Token`` with a
   valid OAuth 2 token with read access to the user profile.  This can be
   enabled for at most one of the OAuth 2 providers and it is disabled by
   default.

GET ``/cosmos/v1``
------------------

*Since v1*

Represents endpoint for the REST API::

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
      ]
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
      "size": <int>,
      "master": { "hostname": <string>, "ipAddress": <string> },
      "slaves" : [
        { "hostname": <string>, "ipAddress": <string> },
        { "hostname": <string>, "ipAddress": <string> },
        ...
      ],
      "users": [
        { "username": <string>, "isSudoer": <boolean>, "sshPublicKey": <string> },
        { "username": <string>, "isSudoer": <boolean>, "sshPublicKey": <string> },
        ...
      ]
    }

State related fields have the same meaning as in ``/cosmos/v1/cluster`` GET
response.

POST ``/cosmos/v1/cluster/<id>/add_user``
-----------------------------------------

*Since v1*

Add a new user to the cluster with id ``<id>``. Request is of the form::

    {
      "username": <string>,
    }

The request must match the following rules.

* The ``<username>`` field must match the handle of an existing user in the platform
* The ``<username>`` field must match the handle of a user that is not a user of the cluster

If all these rules match, the request returns immediately with status 200 OK. The user addition
may take a while, so check the ``users`` field by means of a GET to check the user was added.


POST ``/cosmos/v1/cluster/<id>/remove_user``
--------------------------------------------

*Since v1*

Remove an user from the cluster with id ``<id>``. Request is of the form::

    {
      "username": <string>,
    }

The request must match the following rules.

* The ``<username>`` field must match the handle of an existing user in the platform
* The ``<username>`` field must match the handle of a user that is a user of the cluster
* The ``<username>`` field must match the handle of a user that is not the owner of the cluster

If all these rules match, the request returns immediately with status 200 OK. The user removal
may take a while, so check the ``users`` field by means of a GET to check the user was removed.


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


GET ``/cosmos/v1/storage/auth``
-------------------------------

*Since v1*

Allows identity authentication for the Infinity filesystem. It provides the user identity given
either API credentials or a cluster secret provided as query parameters (respectively ``apiKey``,
``apiSecret`` and ``clusterSecret`` parameters).  For example,
``/cosmos/v1/storage?clusterSecret=<secret>`` can be used to authorize from a cluster secret.

In case of success, a JSON document with the following structure is returned with OK status::

    {
       "user": <string>,
       "group": <string>,
       "accessMask": <string>,
       "origins": <origin policy>
    }

Where ``accessMask`` is a string with a unix-style access mask such as "777" or "077" and
``origins`` can be ``anyHost`` or a JSON array of the allowed IPs (e.g. ``["10.2.0.1", ... ]``).

In case of malformed input or invalid credentials a BAD REQUEST status will be returned with a
message with additional details as body.


GET ``/cosmos/v1/maintenance``
------------------------------

*Since v1*

Determines if the system is in maintenance status. Returns just a boolean payload.


PUT ``/cosmos/v1/maintenance``
------------------------------

*Since v1*

For operator users, allow to enter or leave the maintenance mode by posting a boolean payload.
Other users will get a Forbidden status.

In case of success the maintenance status will change and the new mode will be returned
as a boolean payload with 200 status.


GET ``/cosmos/v1/info``
-----------------------

*Since v1*

General info about the platform for the user performing the request: user details, owned and
accessible clusters and resource information.

In case of success, a 200 status and a payload of the following form is returned::

    {
        "profileId": <int>,
        "handle": <string>,
        "individualQuota": <int>,
        "group": {
            "name": <string>,
            "guaranteedQuota": <int>
        },
        "clusters": {
            "owned": [<string>, <string>, ...],
            "accessible": [<string>, <string>, ...]
        },
        "resources": {
            "groupConsumption": <int>,
            "individualConsumption": <int>,
            "available": <int>,
            "availableForGroup": <int>,
            "availableForUser": <int>
        }
    }

In the case of lacking minimum or maximum quotas the ``individualQuota`` or ``guaranteedQuota``
fields will be missing.


GET ``/cosmos/v1/stats/clusters``
---------------------------------

*Since v1*

For operator users, reports running cluster resource usage.  This is very
useful to get a glimpse on how platform resources are distributed in a given
point in time.

In case of having the required permissions, the result will have a 200 status
and a JSON payload of the form::

    {
      "clusters": [
        {
          "id": <string>,
          "name": <string>,
          "ownerHandle": <string>,
          "size": <int>,
        }, {
          "id": <string>,
          "name": <string>,
          "ownerHandle": <string>,
          "size": <int>,
        }, {
        ...
        }
      ]
    }

Otherwise 401 and 403 status with an error message will be returned.


GET ``/cosmos/v1/stats/machines``
---------------------------------

*Since v1*

For operator users, reports total and available machines.  The results are
grouped by machine profile.

In case of having the required permissions, the result will have a 200 status
and a JSON payload of the form::

    {
      <profile>: {
        "total": <int>,
        "available": <int>
      },
      <profile>: {
        "total": <int>,
        "available": <int>
      }
    }

Where ``<profile>`` identifies a machine type such as "g1-compute" for the
computing nodes of first generation.

Otherwise 401 and 403 status with an error message will be returned.


--------------------------
Resources of the admin API
--------------------------

These resources follow an authentication scheme different for the client API.
Instead of using the pair API id / secret, a different set of credentials are
accepted per authentication realm.  This is configured and enabled on the
`cosmos-api` configuration file.

Authentication
--------------

Requests should use a basic Authorization header as in RFC 2617 in which
username corresponds to the ``authRealm`` being used in the call and the password
is the one provided by the Cosmos team to the realm owners.

POST ``/admin/v1/user``
-----------------------

*Since v1*

Provides a mean for user registration by posting the properties of the newly
created user.  The properties have the following restrictions:

* `authId`: non-empty string that must be unique per authorization realm.
* `authRealm`: identifier of the authorization realm (also a non-empty string).
* `email`: email address to contact the user about maintenance windows or other
  conditions and announcements.
* `handle`: user handle to be used as SSH login. It must be a valid unix login
  (letters and numbers with a leading letter) and at least three characters.
  If this field is not present, one will be generated.
* `sshPublicKey`: must be a public key in the same format SSH stores it
  (`ssh-rsa|ssh-dsa`, the key and the user email).

Sample body::

    {
      "authId": "id",
      "authRealm": "realm",
      "email": "user@host",
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

DELETE ``/admin/v1/user/<realm>/<id>``
--------------------------------------

*Since v1*

Provides a mean for user unregistration by sending a DELETE request.
Note that the realm on the URL should match with the authentication credentials so
each authentication provider can delete only its own users.

In case of success, a 200 response is returned.
Otherwise, one of the following errors will be returned:

* Unauthorized 401
* Forbidden 403
* Not found 404, the user does not exist.
* Internal server error 500, account unregistration failed.
