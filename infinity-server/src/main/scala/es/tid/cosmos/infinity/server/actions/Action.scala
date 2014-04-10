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

package es.tid.cosmos.infinity.server.actions

import spray.http.{HttpResponse, HttpRequest}

import es.tid.cosmos.infinity.server.util.Path
import es.tid.cosmos.infinity.server.authentication.UserProfile

/** An action performed on a Infinity path. */
sealed trait Action {
  val on: Path

  def run(user: UserProfile): HttpResponse = ???
}

object Action extends ActionMapping {

  private val MetadataPath = """/infinityfs/v1/metadata(.*)""".r

  def apply(request: HttpRequest): Action = {
    request.uri.path.toString() match {
      case MetadataPath(path) =>
        makeMetadataAction(Path.absolute(path), request)
      case _ =>
        opQueryParam(request) match {
          case "OPEN" => Open(request)
          case "GETFILESTATUS" => GetFileStatus(request)
          case "LISTSTATUS" => ListStatus(request)
          case "GETCONTENTSUMMARY" => GetContentSummary(request)
          case "GETFILECHECKSUM" => GetFileChecksum(request)
          case "GETHOMEDIRECTORY" => GetHomeDirectory(request)
          case "GETDELEGATIONTOKEN" => GetDelegationToken(request)
          case "CREATE" => Create(request)
          case "MKDIRS" => Mkdirs(request)
          case "RENAME" => Rename(request)
          case "SETREPLICATION" => SetReplication(request)
          case "SETOWNER" => SetOwner(request)
          case "SETPERMISSION" => SetPermission(request)
          case "SETTIMES" => SetTimes(request)
          case "RENEWDELEGATIONTOKEN" => RenewDelegationToken(request)
          case "CANCELDELEGATIONTOKEN" => CancelDelegationToken(request)
          case "APPEND" => Append(request)
          case "DELETE" => Delete(request)
          case op => throw new IllegalArgumentException(s"unknown operation '$op' in $request")
        }
    }
  }

  case class GetMetadata(on: Path) extends Action

  private def makeMetadataAction(path: Path, request: HttpRequest): Action = GetMetadata(path)

  /** A OPEN action on a Infinity path. */
  case class Open(
    on: Path,
    offset: Option[Long] = None,
    length: Option[Long] = None,
    buffersize: Option[Int] = None) extends Action

  object Open {

    def apply(request: HttpRequest): Open =
      withGetBasicProps(request) { path =>
        Open(
          on = path,
          offset = queryParamOpt(request, "offset").map(_.toLong),
          length = queryParamOpt(request, "length").map(_.toLong),
          buffersize = queryParamOpt(request, "buffersize").map(_.toInt)
        )
      }
  }

  /** A GETFILESTATUS action on a Infinity path. */
  case class GetFileStatus(on: Path) extends Action

  object GetFileStatus {

    def apply(request: HttpRequest): GetFileStatus =
      withGetBasicProps(request)(GetFileStatus.apply)
  }

  /** A LISTSTATUS action on a Infinity path. */
  case class ListStatus(on: Path) extends Action

  object ListStatus {

    def apply(request: HttpRequest): ListStatus =
      withGetBasicProps(request)(ListStatus.apply)
  }

  /** A GETCONTENTSUMMARY action on a Infinity path. */
  case class GetContentSummary(on: Path) extends Action

  object GetContentSummary {

    def apply(request: HttpRequest): GetContentSummary =
      withGetBasicProps(request)(GetContentSummary.apply)
  }

  /** A GETFILECHECKSUM action on a Infinity path. */
  case class GetFileChecksum(on: Path) extends Action

  object GetFileChecksum {

    def apply(request: HttpRequest): GetFileChecksum =
      withGetBasicProps(request)(GetFileChecksum.apply)
  }

  /** A GETHOMEDIRECTORY action on a Infinity path. */
  case class GetHomeDirectory(on: Path) extends Action

  object GetHomeDirectory {

    def apply(request: HttpRequest): GetHomeDirectory =
      withGetBasicProps(request)(GetHomeDirectory.apply)
  }

  /** A GETDELEGATIONTOKEN action on a Infinity path. */
  case class GetDelegationToken(
    on: Path,
    renewer: String) extends Action

  object GetDelegationToken {

    def apply(request: HttpRequest): GetDelegationToken =
      withGetBasicProps(request) { path =>
        GetDelegationToken(
          on = path,
          renewer = queryParam(request, "renewer")
        )
      }
  }

  /** A CREATE action on a Infinity path. */
  case class Create(
    on: Path,
    overwrite: Option[Boolean] = None,
    blocksize: Option[Long] = None,
    replication: Option[Short] = None,
    permission: Option[Int] = None,
    buffersize: Option[Int] = None) extends Action

  object Create {

    def apply(request: HttpRequest): Create =
      withPutBasicProps(request) { path =>
        Create(
          on = path,
          overwrite = queryParamOpt(request, "overwrite").map(_.toBoolean),
          blocksize = queryParamOpt(request, "blocksize").map(_.toLong),
          replication = queryParamOpt(request, "replication").map(_.toShort),
          permission = queryParamOpt(request, "permission").map(Integer.parseInt(_, 8)),
          buffersize = queryParamOpt(request, "buffersize").map(_.toInt)
        )
      }
  }

  /** A MKDIRS action on a Infinity path. */
  case class Mkdirs(
    on: Path,
    permission: Option[Int] = None) extends Action

  object Mkdirs {

    def apply(request: HttpRequest): Mkdirs =
      withPutBasicProps(request) { path =>
        Mkdirs(
          on = path,
          permission = queryParamOpt(request, "permission").map(Integer.parseInt(_, 8))
        )
      }
  }

  /** A RENAME action on a Infinity path. */
  case class Rename(on: Path, destination: String) extends Action

  object Rename {

    def apply(request: HttpRequest): Rename =
      withPutBasicProps(request) { path =>
        Rename(
          on = path,
          destination = queryParam(request, "destination")
        )
      }
  }

  /** A SETREPLICATION action on a Infinity path. */
  case class SetReplication(
    on: Path,
    replication: Option[Short] = None) extends Action

  object SetReplication {

    def apply(request: HttpRequest): SetReplication =
      withPutBasicProps(request) { path =>
        SetReplication(
          on = path,
          replication = queryParamOpt(request, "replication").map(_.toShort)
        )
      }
  }

  /** A SETOWNER action on a Infinity path. */
  case class SetOwner(
    on: Path,
    owner: Option[String] = None,
    group: Option[String] = None) extends Action

  object SetOwner {

    def apply(request: HttpRequest): SetOwner =
      withPutBasicProps(request) { path =>
        SetOwner(
          on = path,
          owner = queryParamOpt(request, "owner"),
          group = queryParamOpt(request, "group")
        )
      }
  }

  /** A SETPERMISSION action on a Infinity path. */
  case class SetPermission(
    on: Path,
    permission: Option[Int] = None) extends Action

  object SetPermission {

    def apply(request: HttpRequest): SetPermission =
      withPutBasicProps(request) { path =>
        SetPermission(
          on = path,
          permission = queryParamOpt(request, "permission").map(Integer.parseInt(_, 8))
        )
      }
  }

  /** A SETTIMES action on a Infinity path. */
  case class SetTimes(
    on: Path,
    modificationTime: Option[Long] = None,
    accessTime: Option[Long] = None) extends Action

  object SetTimes {

    def apply(request: HttpRequest): SetTimes =
      withPutBasicProps(request) { path =>
        SetTimes(
          on = path,
          modificationTime = queryParamOpt(request, "modificationtime").map(_.toLong),
          accessTime = queryParamOpt(request, "accesstime").map(_.toLong)
        )
      }
  }

  /** A RENEWDELEGATIONTOKEN action on a Infinity path. */
  case class RenewDelegationToken(
    on: Path, token: String) extends Action

  object RenewDelegationToken {

    def apply(request: HttpRequest): RenewDelegationToken =
      withPutBasicProps(request) { path =>
        RenewDelegationToken(
          on = path,
          token = queryParam(request, "token")
        )
      }
  }

  /** A CANCELDELEGATIONTOKEN action on a Infinity path. */
  case class CancelDelegationToken(on: Path, token: String) extends Action

  object CancelDelegationToken {

    def apply(request: HttpRequest): CancelDelegationToken =
      withPutBasicProps(request) { path =>
        CancelDelegationToken(
          on = path,
          token = queryParam(request, "token")
        )
      }
  }

  /** A APPEND action on a Infinity path. */
  case class Append(
    on: Path,
    buffersize: Option[Int] = None) extends Action

  object Append {

    def apply(request: HttpRequest): Append =
      withPostBasicProps(request) { path =>
        Append(
          on = path,
          buffersize = queryParamOpt(request, "buffersize").map(_.toInt)
        )
      }
  }

  /** A DELETE action on a Infinity path. */
  case class Delete(
    on: Path,
    recursive: Option[Boolean] = None) extends Action

  object Delete {

    def apply(request: HttpRequest): Delete =
      withDeleteBasicProps(request) { path =>
        Delete(
          on = path,
          recursive = queryParamOpt(request, "recursive").map(_.toBoolean)
        )
      }
  }
}
