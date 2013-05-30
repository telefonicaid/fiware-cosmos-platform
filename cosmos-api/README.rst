==========
Cosmos API
==========

This module exposes Cosmos functionality as a REST API and offers a minimal web
application for managing some account details.

---------------
Management page
---------------

The web application is integrated with the TU|Id service as authentication
provider so if you plan to use Cosmos, you first need to get a TU account.  The
first time you access Cosmos you will be asked to authorize the service and you
should register you SSH public key.  Follow the on-screen instructions.

Once registered you will get your API key and API secret to configure the
client-side tools.

--------
REST API
--------

Despite Cosmos functionality is exposed as a RESTful API you don't need to
write your own client to use it, you can leverage the `cosmos-cli` module and
operate from your command line.  However, API details are roughly described
here just in case a tightly integrated solution.

Authentication
--------------

Requests should use a basic Authorization header as in RFC 2617 in which
username corresponds with the API key and the password with the API secret.

Resources
---------

- GET `/cosmos`.

  Represents general API information as JSON::

    {
      "version": "X.Y.Z"
    }

- GET `/cosmos/cluster`

  Represents all the user clusters. Listing is of the form::

    {
      "clusters": [
        { "id": <string>, "href": <url> },
        { "id": <string>, "href": <url> }
      ]
    }

- POST `/cosmos/cluster`

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
    }

- GET  `/cosmos/cluster/<id>`

  Consult details of the cluster with id `<id>`. Body as follows::

    {
      "id": <string>,
      "stateDescription": <string>,
      "name": <string>,
      "state": <string>,
      "href": <string>,
      "size": <int>
    }

- POST `/cosmos/cluster/<id>/terminate`

  Terminates the cluster with id `<id>`. Returns immediatly so check the
  `state` field by means of a GET to check termination status.
