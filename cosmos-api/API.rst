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

---------
Resources
---------

GET ``/cosmos/v1``
------------------

*Since v1*

Represents general API information as JSON::

    {
      "version": "X.Y.Z"
    }

GET ``/cosmos/v1/cluster``
--------------------------

*Since v1*

Represents all the user clusters. Listing is of the form::

    {
      "clusters": [
        { "id": <string>, "href": <url> },
        { "id": <string>, "href": <url> }
      ]
    }

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
    }

GET  ``/cosmos/v1/cluster/<id>``
--------------------------------

*Since v1*

Consult details of the cluster with id ``<id>``. Body as follows::

    {
      "id": <string>,
      "stateDescription": <string>,
      "name": <string>,
      "state": <string>,
      "href": <string>,
      "size": <int>
    }

POST ``/cosmos/v1/cluster/<id>/terminate``
------------------------------------------

*Since v1*

Terminates the cluster with id ``<id>``. Returns immediatly so check the
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
