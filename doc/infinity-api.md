# Infinity REST protocol

## Introduction

The Infinity REST protocol is the communication mechanism provided by Infinity
that allows Cosmos users to consume the storage services provided by the
platform. It is designed as a REST API over HTTP that provides a set of
operations to manage the filesystem resources of Infinity.

## Authentication

Infinity REST protocol uses two different authentication mechanisms in order
to support both user and cluster credentials.

 * For user credentials, HTTP Basic Access Authentication is used according to
   [RFC 2617](https://tools.ietf.org/html/rfc2617).
 * For cluster credentials, OAuth bearer token is used according to [RFC
   6750](http://tools.ietf.org/html/rfc6750).

The following example shows a request and its response for a request without credentials.

```
GET http://example.com/infinityfs/v1/...
```

```
HTTP/1.1 401 Unauthorized
WWW-Authenticate: Basic realm="Infinity"
WWW-Authenticate: Bearer realm="Infinity"
```

Two `WWW-Authenticate` headers are included in the response, indicating the
client may use any of them to authenticate further requests. Then, the client
could retry the request with basic authentication using his API key and secret
pair.

```
GET http://example.com/infinityfs/v1/...
Authorization: Basic R2FuZGFsZjpGdWNrWW91U2F1cm9uIQo=
```

```
HTTP/1.1 200 OK

...
```

If the client wants to authenticate the request using a cluster secret, the
following would be used.

```
GET http://example.com/infinityfs/v1/...
Authorization: Bearer 4641EC92FED844439106AEE55ED900DF
```

## Server Roles

There are two different roles a Infinity server can adopt.

 * Metadata server, provides actions on file or directory metadata.
 * Content server, provides actions on file contents.

### Metadata Server

The metadata server implements a set of operations in order to manage metadata
of files and directories. That includes:

 * Create new files or directories
 * Move files or directories
 * Retrieve the metadata of files or directories (that includes the contents of a directory)
 * Modify some fields of the metadata as owner, group and permissions.

### Content Server

The content server implements a set of operations to move file contents
between client and server and vice versa. In other words, read or write a
file. That includes:

 * Read the contents of a file
 * Append new contents to an existing file
 * Write content to a file, replacing any previous data it could store

## Filesystem Operations

There are two subsets of actions that can be performed on Infinity files:
those which manages file and directory metadata and those which manages file
contents. As you may guess, the former is served by Metadata Server and the
latter by Content Server.

The different subsets are access through different resource trees:

 * Those resources placed under `/infinityfs/v1/metadata` are representing the
   metadata of the files or directories.
 * Those resources placed under `/infinityfs/v1/content` are representing the
   contents of the files or directories.

_Note: in future releases, it may be useful to make metadata server to serve
content resources by redirecting the requests to the appropriate content
server. In case of file reading, this redirection can be based upon the block
number to be retrieved passed as a query parameter. In case of file reading, a
`Expect: 100-continue` header can be used by the client in order to wait for
the server to redirect to an appropriate content server. In the first version,
it will be simpler that the metadata references the appropriate content URL
that points to a content server in a round-robin fashion._

## Metadata Operations

### Retrieve file metadata

The metadata of any file can be retrieved by sending a GET request to the
Infinity Server as the following example shows.

```
GET http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt
```

As response, the server returns a JSON object describing the metadata of the file.

```
HTTP/1.1 200 OK
{
  "path" : "/usr/gandalf/spells.txt",
  "type" : "file",
  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt",
  "content" : "http://example.com/infinityfs/v1/content/usr/gandalf/spells.txt",
  "owner" : "gandalf",
  "group" : "istari",
  "permissions" : "600",
  "size" : 45566918656,
  "modificationTime" : "2014-04-08T12:31:45+0100",
  "accessTime" : "2014-04-08T12:45:22+0100",
  "blockSize" : 65536,
  "replication" : 3
}
```

The metadata object have the following fields.

 * `path`, which indicates the path in the filesystem where the file is
   located
 * `type`, which indicates that the resource is a file (`file`)
 * `metadata`, which indicates the URL where the metadata resource is located.
   This may be not present temporarily in case of the file is being uploaded
   and its blocks are still not available.
 * `content`, which indicates the URL where the content resource is located.
   The content server may vary from one request to the next due to load
   balancing purposes.
 * `owner` indicates the user who owns the file
 * `group` indicates the group associated to the file
 * `permissions`, which indicates the UNIX-like permissions of the file
 * `size`, which indicates the size of the file in bytes
 * `modificationTime`, which indicates the last time when the file was
   modified
 * `accessTime`, which indicates the last time when the file was accessed
 * `blockSize`, which indicates the size of the block for this file in bytes
 * `replication`, which determines the replication factor for this file

### Retrieve directory metadata

It is possible to invoke a GET operation on the metadata resource for a
directory as well.

```
GET http://example.com/infinityfs/v1/metadata/usr/gandalf
```
Again, the server returns the metadata of that directory which now includes
its contents.

```
HTTP/1.1 200 OK
{
  "path" : "/usr/gandalf",
  "type" : "directory",
  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf",
  "owner" : "gandalf",
  "group" : "istari",
  "permissions" : "755",
  "size" : 0,
  "modificationTime" : "2014-04-08T12:31:45+0100",
  "accessTime" : "2014-04-08T12:45:22+0100",
  "content" : [
    {
      "path" : "/usr/gandalf/spells.txt",
      "type" : "file",
      "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/spells.txt",
      "owner" : "gandalf",
      "group" : "istari",
      "permissions" : "600",
      "size" : 45566918656,
      "modificationTime" : "2014-04-08T12:41:34+0100",
      "accessTime" : "2014-04-08T12:54:32+0100",
      "blockSize" : 65536,
      "replication" : 3
    },
    {
      "path" : "/usr/gandalf/enemies",
      "type" : "directory",
      "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies",
      "owner" : "gandalf",
      "group" : "istari",
      "permissions" : "750",
      "size" : 0,
      "modificationTime" : "2014-04-08T12:55:45+0100",
      "accessTime" : "2014-04-08T13:01:22+0100",
      "blockSize" : 0,
      "replication" : 0
    }
  ]
}
```

The metadata object is very similar to the one used for files. It has the
following fields.

 * `path`, which indicates the path in the filesystem where the directory is
   located
 * `type`, which indicates that the resource is a directory (`directory`)
 * `metadata`, which indicates the URL where the metadata resource is located
 * `owner` indicates the user who owns the directory
 * `group` indicates the group associated to the directory
 * `permissions`, which indicates the UNIX-like permissions of the directory
 * `size`, which is always zero
 * `modificationTime`, which indicates the last time when the directory was
   modified
 * `accessTime`, which indicates the last time when the directory was accessed
 * `content`, which provides an array of abstract metadata objects, one per
   directory entry. These objects *do not have* a `content` field. See [data
   type specification](Infinity-rest-protocol.md#data-type-specification) for
   further details.

### Create a new file

The way a new file is created is by posting a metadata creation command object
in an existing directory as the following example shows.

```
POST http://example.com/infinityfs/v1/metadata/usr/gandalf

{
  "action" : "mkfile",
  "name" : "enemies.csv",
  "permissions" : "640",
  "replication" : 2,
  "blockSize" : 67108864
}
```

In this example, the HTTP request contains a body that describes an action to
create a new file named `enemies.csv`, with permissions set to 640, a
replication factor of 2 (if omitted, the default value is used) and block size
of 64MiB (67108864 bytes). If everything goes well, the server responds with
_201 Created_ status code and the metadata of the newly created file in its
body.

```
HTTP/1.1 201 Created

{
  "path" : "/usr/gandalf/enemies.csv",
  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies.csv",
  "content" : "http://data03.example.com/infinityfs/v1/content/usr/gandalf/enemies.csv",
  "type" : "file",
  "owner" : "gandalf",
  "group" : "istari",
  "permissions" : "640",
  "size" : 0,
  "modificationTime" : "2014-04-08T12:31:45+0100",
  "accessTime" : "2014-04-08T12:45:22+0100",
  "blockSize" : 67108864,
  "replication" : 3
}
```

If a file or directory already exists in that path with the same name, the
Infinity Server shall respond with _409 Conflict_ return code and the file
will not be created.

Upon its success creation, the file will have a length of zero with no
content. It's possible to fill the initial content by [appending new data to
the file](#appending-data-to-a-existing-file).

### Create a new directory

The creation of a new directory is very similar to the creation of a new file.
But, in case of directory, the creation command looks like:

```
POST http://example.com/infinityfs/v1/metadata/usr/gandalf

{
  "action" : "mkdir",
  "name" : "enemies",
  "permissions" : "777"
}
```

The server will respond with the same kind of request as for a file.

```
HTTP/1.1 201 Created

{
  "path" : "/usr/gandalf/enemies",
  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies",
  "type" : "directory",
  "owner" : "gandalf",
  "group" : "istari",
  "permissions" : "777",
  "size" : 0,
  "modificationTime" : "2014-04-08T12:31:45+0100",
  "accessTime" : "2014-04-08T12:45:22+0100",
  "content" : []
}
```

### Move a file or directory

It's possible to use a creation command to move an existing file or directory
into a new location, as the following example shows.

```
POST http://example.com/infinityfs/v1/metadata/usr/gandalf

{
  "action" : "move",
  "name" : "enemies",
  "from" : "/usr/sauron/friends",
}
```

The `action` field will have a `move` value. The `from` field indicates the
path of the original file or directory that is being moved.

The server will respond with the metadata of the new file or directory.

```
HTTP/1.1 201 Created

{
  "path" : "/usr/gandalf/enemies",
  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies",
  "type" : "directory",
  "owner" : "gandalf",
  "group" : "istari",
  "permissions" : "777",
  "size" : 0,
  "modificationTime" : "2014-04-08T12:31:45+0100",
  "accessTime" : "2014-04-08T12:45:22+0100",
  "content" : [ ... ]
}
```

### Changing owner, group and permissions

It's possible to modify the owner, the group and the permissions of a file or
directory by posting the appropriate commands using a POST request.

The following example shows how to change the owner of a file.

```
POST http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies

{
  "action" : "chown",
  "owner" : "theoden"
}
```

If succeed, the server will respond with the new file metadata:

```
HTTP/1.1 200 OK

{
  "path" : "/usr/gandalf/enemies",
  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies",
  "type" : "directory",
  ...
}
```

Analogously, we may change the group and permissions using `chgrp` and
`chmod`, respectively, as in:

```
POST http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies

{
  "action" : "chgrp",
  "group" : "valar"
}
```

```
POST http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies

{
  "action" : "chmod",
  "permissions" : "755"
}
```

### Deleting a file or directory

A file or directory can be deleted by sending a DELETE request on its metadata
object:

```
DELETE http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies
```

As response, the server returns:

```
HTTP/1.1 204 No Content
Content-Length: 0
```

Directories must be empty before deleted unless a recursive delete is
requested. That can be done with `recursive=true` query parameter:

```
DELETE http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies?recursive=true
```

## Content Operations

### Retrieving data from an existing file

Retrieving the data of an existing file is as simple as sending a GET request
to the Infinity Server on the URL of the file content provided in the metadata
object. In order to guarantee load balancing, it's very important to follow
that URL rather than composing it synthetically. When file metadata is
obtained, an appropriate content server is selected and the `content` field is
filled with its URL.

```
GET http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies.csv
```

```
HTTP/1.1 200 OK

{
  "path" : "/usr/gandalf/enemies.csv",
  "metadata" : "http://example.com/infinityfs/v1/metadata/usr/gandalf/enemies.csv",
  "content" : "http://data07.example.com/infinityfs/v1/content/usr/gandalf/enemies.csv",
  "type" : "file",
  ...
}
```

With this, we may follow the URL provided in `content` field:

```
GET http://data07.example.com/infinityfs/v1/content/usr/gandalf/enemies.csv
```

As response, the server returns the contents of the file.

```
HTTP/1.1 200 OK
Content-Length: 106

Sauron,Maia,The Dark Lord
Saruman,Istari,The White Wizard
Peregrin Tuk,Hobbit,That little unpleasant rat
```

It's possible to obtain a slice or block of the file content by using the
following query parameters:

 * `offset`, which have a unsigned long value indicating where to start
   reading. When omitted, it has a default value of 0. It can be combined with
   `length`.
 * `length`, which have a unsigned long value indicating the maximum length to
   read. When omitted, there is no maximum length. It can be combined with
   `offset`.

### Appending data to a existing file

For any existing file, it is possible to append new data by invoking a POST
request on the appropriate content resource provided in the file metadata
object.

The following example shows how to append bytes to an existing file using the
URL obtained from the `content` field of the metadata object.

```
POST http://data06.example.com/infinityfs/v1/content/usr/gandalf/enemies.csv

Sauron,Maia,The Dark Lord
Saruman,Istari,The White Wizard
Peregrin Tuk,Hobbit,That little unpleasant rat
```

As response, the server will return:

```
HTTP/1.1 204 No Content
Content-Length: 0

```

### Overwrite the file contents

It's possible to remove all the contents of an existing file and replace it by
new data by using a PUT request instead of POST.

```
PUT http://data06.example.com/infinityfs/v1/content/usr/gandalf/enemies.csv

Sauron,Maia,The Dark Lord
Saruman,Istari,The White Wizard
Aragorn,Dunedain,He owes me some bucks
```

The server will respond with:

```
HTTP/1.1 204 No Content
Content-Length: 0

```

## Formal Specification

### Operations

The following lexic elements are used in this section.

* `<medatada-base-url> := http://<metadata-server>/infinityfs/v1/metadata`
* `<content-base-url> := http://<content-server>/infinityfs/v1/content`

**Retrieve file or directory metadata**

 * Request
   * **URL**: `<medatada-base-url>/<path>`
   * **Method**: GET
   * **Query Params**: None
   * **Body**: None
 * Success response
   * **Status code**: 200 OK
   * **Body**: _File metadata object_ for files, _directory metadata object_
     for directories.
 * Not found response
   * **Reason**: There is no file or directory for path specified
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user haven't the appropriate permissions to retrieve the
     file or directory metadata
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_

**Create a new file or directory**

 * Request
   * **URL**: ```<medatada-base-url>/<parent-directory>```
   * **Method**: POST
   * **Query Params**: None
   * **Body**: _Mkfile action | Mkdir action_
 * Success response
   * **Status code**: 201 Created
   * **Body**: _File metadata object_ | _Directory metadata object_
 * Not found response
   * **Reason**: The parent directory does not exist
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user doesn't have the appropriate permissions to create
     the file or directory in that target
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_
 * Already exists response
   * **Reason**: There is a directory entry with the given file or directory
     name
   * **Status code**: 409 Conflict
   * **Body**: _Error descriptor_
 * Parent not a directory response
   * **Reason**: The parent path where the new file or directory is to be
     created is not a directory
   * **Status code**: 422 Unprocessable Entity
   * **Body**: _Error descriptor_
 * Bad request response
   * **Reason**: The body of the request is not syntactically well-formed.
   * **Status code**: 400 Bad Request
   * **Body**: _Error descriptor_

**Move a file or directory**

 * Request
   * **URL**: ```<medatada-base-url>/<parent-directory>```
   * **Method**: POST
   * **Query Params**: None
   * **Body**: _Move action_
 * Success response
   * **Status code**: 201 Created
   * **Body**: _File metadata object_ | _Directory metadata object_
 * Not found response
   * **Reason**: The source file or directory does not exist
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user doesn't have the appropriate permissions to move the
     file or directory into that target
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_
 * Already exists response
   * **Reason**: There is a directory entry with the given file name
   * **Status code**: 409 Conflict
   * **Body**: _Error descriptor_
 * Parent not a directory response
   * **Reason**: The parent path where the new file or directory is to be
     moved is not a directory
   * **Status code**: 422 Unprocessable Entity
   * **Body**: _Error descriptor_
 * Bad request response
   * **Reason**: The body of the request is not syntactically well-formed.
   * **Status code**: 400 Bad Request
   * **Body**: _Error descriptor_

**Change owner, group and set permissions**

 * Request
   * **URL**: ```<medatada-base-url>/<file-or-directory>```
   * **Method**: POST
   * **Query Params**: None
   * **Body**: _Chown action | Chgrp action | Chmod action_
 * Success response
   * **Status code**: 204 No Content
   * **Body**: _File metadata_ | _Directory metadata_
 * Not found response
   * **Reason**: The target file or directory does not exist
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user doesn't have the appropriate permissions to change
     the file or directory metadata
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_
 * Bad request response
   * **Reason**: The body of the request is not syntactically well-formed.
   * **Status code**: 400 Bad Request
   * **Body**: _Error descriptor_

**Delete a file or directory**

 * Request
   * **URL**: ```<medatada-base-url>/<file-or-directory>```
   * **Method**: DELETE
   * **Query Params**:
     * `recursive=<boolean>`, indicates whether the removal shall be done
       recursively in case of directories.
   * **Body**: None
 * Success response
   * **Status code**: 204 No Content
   * **Body**: None
 * Not found response
   * **Reason**: The target file or directory does not exist
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user doesn't have the appropriate permissions to remove
     the file or directory
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_
 * Bad request response
   * **Reason**: The query params are not valid
   * **Status code**: 400 Bad Request
   * **Body**: _Error descriptor_

**Retrieve data from file**

 * Request
   * **URL**: ```<content-base-url>/<file>```
   * **Method**: GET
   * **Query Params**:
     * `offset=<natural>`, indicates the position in bytes where the read will
       start (the first byte). If omitted, zero (_0_) will be used. This
       parameter can be combined with _length_.
     * `length=<positive>`, indicates the maximum amount of bytes to be read.
       The response will contain the bytes from _offset_ to _offset+length_ or
       the end of the file, what comes first. This parameter can be combined
       with _offset_.
   * **Body**: None
 * Success response
   * **Status code**: 200 OK
   * **Body**: The content part of the file that was requested
 * Not found response
   * **Reason**: The target file or directory does not exist
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user doesn't have the appropriate permissions to read the
     file.
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_
 * Bad request response
   * **Reason**: The restrictions of the query params are not satisfied
   * **Status code**: 400 Bad Request
   * **Body**: _Error descriptor_

**Append data to file**

 * Request
   * **URL**: ```<content-base-url>/<file>```
   * **Method**: POST
   * **Query Params**: None
   * **Body**: The content to be appended to the file
 * Success response
   * **Status code**: 204 No Content
   * **Body**: None
 * Not found response
   * **Reason**: The target file or directory does not exist
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user doesn't have the appropriate permissions to write on
     the file.
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_

**Overwrite file contents**

 * Request
   * **URL**: ```<content-base-url>/<file>```
   * **Method**: PUT
   * **Query Params**: None
   * **Body**: The new content of the file
 * Success response
   * **Status code**: 204 No Content
   * **Body**: None
 * Not found response
   * **Reason**: The target file or directory does not exist
   * **Status code**: 404 Not Found
   * **Body**: _Error descriptor_
 * Unauthorized response
   * **Reason**: The user doesn't have the appropriate permissions to write on
     the file.
   * **Status code**: 403 Forbidden
   * **Body**: _Error descriptor_


### Data type specification

In this section, the different objects used in the request and response
payloads are specified. A JSON-like sintax is used in which the field name is
expressed as is and the field value indicates the expected type or value.

* Literal values will be expressed as is. E.g., "file" means a literal string
  `"file"`.
* Types are expressed with angles. E.g., <string>, <positive>, <datetime>...
  The types used in this section are described as follows.
  * ```<string>```, a string of characters
  * ```<positive>```, a unsigned integer (no decimal part) number greater than
    zero (zero is not included)
  * ```<natural>```, a unsigned integer (no decimal part) number greater than
    or equal to zero (zero is included)
  * ```<datetime>```, a instant in time expressed in
    [RFC 822](http://www.ietf.org/rfc/rfc0822.txt)
  * ```<permissions>```, a string with three octal digits representing the
    classical UNIX permission mask (e.g., "755", "640"...)
  * ```<url>```, a string representing a URL as specified in
    [RFC 1738](http://www.rfc-editor.org/rfc/rfc1738.txt)

If a field accepts different types or values, each one will be separated with
a vertical line symbol (`|`).

**Abstract metadata object**

An abstract metadata object is a JSON document that provides the common
attributes for file and directory metadata. It has the following form.

```
{
  "path" : <string>,
  "type" : "file" | "directory",
  "metadata" : <url>,
  "owner" : <string>,
  "group" : <string>,
  "modificationTime" : <datetime>,
  "accessTime" : <datetime>,
  "permissions" : <permissions>,
  "size" : <natural>
}
```

**File metadata object**

A file metadata object is a JSON document with the following contents.

```
{
  "path" : <string>,
  "type" : "file",
  "metadata" : <url>,
  "content" : <url> (optional, content temporarily unavailable if missing),
  "owner" : <string>,
  "group" : <string>,
  "modificationTime" : <datetime>,
  "accessTime" : <datetime>,
  "replication" : <positive>,
  "blockSize" : <positive>,
  "permissions" : <permissions>,
  "size" : <natural>
}
```

**Directory metadata object**

A directory metadata object is a JSON document with the following contents.

```
{
  "path" : <string>,
  "type" : "directory",
  "metadata" : <url>,
  "content" : [ <directory-entry-object>* ],
  "owner" : <string>,
  "group" : <string>,
  "modificationTime" : <datetime>,
  "accessTime" : <datetime>,
  "permissions" : <permissions>,
  "size" : 0
}
```

**Directory entry object**

Directory entries represent both files and directories contained by a given
directory and lack a `content` attribute to avoid be arbitrarily nested.

```
{
  "path" : <string>,
  "type" : "file" | "directory",
  "metadata" : <url>,
  "owner" : <string>,
  "group" : <string>,
  "modificationTime" : <datetime>,
  "accessTime" : <datetime>,
  "replication" : <positive>,
  "blockSize" : <positive>,
  "permissions" : <permissions>,
  "size" : <natural>
}
```

When representing a file, `replication`, `blockSize` and `size` will be 0.

**Mkfile action**

A mkfile action object is a JSON document with the following contents.

```
{
  "action" : "mkfile",
  "name" : <string>,
  "permissions" : <permissions>,
  "replication" : <positive> (optional),
  "blockSize" : <positive> (optional)
}
```

**Mkdir action**

A mkdir action object is a JSON document with the following contents.

```
{
  "action" : "mkdir",
  "name" : <string>,
  "permissions" : <permissions>
}
```

**Move action**

A move action object is a JSON document with the following contents.

```
{
  "action" : "move",
  "name" : <string>,
  "from" : <string>
  "permissions" : <permissions>
}
```

**Chown action**

A chown action object is a JSON document with the following contents.

```
{
  "action" : "chown",
  "owner" : <string>
}
```

**Chgrp action**

A chgrp action object is a JSON document with the following contents.

```
{
  "action" : "chgrp",
  "group" : <string>
}
```

**Chmod action**

A chmod action object is a JSON document with the following contents.

```
{
  "action" : "chmod",
  "permissions" : <permissions>
}
```

**Error descriptor**

An error descriptor is a JSON document with the following contents.

```
{
  "errorCode" : <string>,
  "cause" : <string>
}
```
