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

import java.net.InetSocketAddress

import spray.http.HttpRequest

import es.tid.cosmos.infinity.server.authentication.Credentials
import es.tid.cosmos.infinity.server.util.Path

/** An action performed on a Infinity path. */
sealed trait Action {
  val on: Path
  val credentials: Credentials
}

object Action extends ActionMapping {

  def apply(remoteAddress: InetSocketAddress, request: HttpRequest): Action =
    opQueryParam(request) match {
      case "OPEN" => Open(remoteAddress, request)
      case "GETFILESTATUS" => GetFileStatus(remoteAddress, request)
      case "LISTSTATUS" => ListStatus(remoteAddress, request)
      case "GETCONTENTSUMMARY" => GetContentSummary(remoteAddress, request)
      case "GETFILECHECKSUM" => GetFileChecksum(remoteAddress, request)
      case "GETHOMEDIRECTORY" => GetHomeDirectory(remoteAddress, request)
      case "GETDELEGATIONTOKEN" => GetDelegationToken(remoteAddress, request)
      case "CREATE" => Create(remoteAddress, request)
      case "MKDIRS" => Mkdirs(remoteAddress, request)
      case "RENAME" => Rename(remoteAddress, request)
      case "SETREPLICATION" => SetReplication(remoteAddress, request)
      case "SETOWNER" => SetOwner(remoteAddress, request)
      case "SETPERMISSION" => SetPermission(remoteAddress, request)
      case "SETTIMES" => SetTimes(remoteAddress, request)
      case "RENEWDELEGATIONTOKEN" => RenewDelegationToken(remoteAddress, request)
      case "CANCELDELEGATIONTOKEN" => CancelDelegationToken(remoteAddress, request)
      case "APPEND" => Append(remoteAddress, request)
      case "DELETE" => Delete(remoteAddress, request)
      case op => throw new IllegalArgumentException(s"unknown operation '$op' in $request")
    }

  /** A OPEN action on a Infinity path. */
  case class Open(
    on: Path,
    credentials: Credentials,
    offset: Option[Long] = None,
    length: Option[Long] = None,
    buffersize: Option[Int] = None) extends Action

  object Open {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): Open =
      withGetBasicProps(remoteAddress, request) { (path, credentials) =>
        Open(
          on = path,
          credentials,
          offset = queryParamOpt(request, "offset").map(_.toLong),
          length = queryParamOpt(request, "length").map(_.toLong),
          buffersize = queryParamOpt(request, "buffersize").map(_.toInt)
        )
      }
  }

  /** A GETFILESTATUS action on a Infinity path. */
  case class GetFileStatus(on: Path, credentials: Credentials) extends Action

  object GetFileStatus {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): GetFileStatus =
      withGetBasicProps(remoteAddress, request)(GetFileStatus.apply)
  }

  /** A LISTSTATUS action on a Infinity path. */
  case class ListStatus(on: Path, credentials: Credentials) extends Action

  object ListStatus {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): ListStatus =
      withGetBasicProps(remoteAddress, request)(ListStatus.apply)
  }

  /** A GETCONTENTSUMMARY action on a Infinity path. */
  case class GetContentSummary(on: Path, credentials: Credentials) extends Action

  object GetContentSummary {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): GetContentSummary =
      withGetBasicProps(remoteAddress, request)(GetContentSummary.apply)
  }

  /** A GETFILECHECKSUM action on a Infinity path. */
  case class GetFileChecksum(on: Path, credentials: Credentials) extends Action

  object GetFileChecksum {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): GetFileChecksum =
      withGetBasicProps(remoteAddress, request)(GetFileChecksum.apply)
  }

  /** A GETHOMEDIRECTORY action on a Infinity path. */
  case class GetHomeDirectory(on: Path, credentials: Credentials) extends Action

  object GetHomeDirectory {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): GetHomeDirectory =
      withGetBasicProps(remoteAddress, request)(GetHomeDirectory.apply)
  }

  /** A GETDELEGATIONTOKEN action on a Infinity path. */
  case class GetDelegationToken(
    on: Path,
    credentials: Credentials,
    renewer: String) extends Action

  object GetDelegationToken {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): GetDelegationToken =
      withGetBasicProps(remoteAddress, request) { (path, credentials) =>
        GetDelegationToken(
          on = path,
          credentials,
          renewer = queryParam(request, "renewer")
        )
      }
  }

  /** A CREATE action on a Infinity path. */
  case class Create(
    on: Path,
    credentials: Credentials,
    overwrite: Option[Boolean] = None,
    blocksize: Option[Long] = None,
    replication: Option[Short] = None,
    permission: Option[Int] = None,
    buffersize: Option[Int] = None) extends Action

  object Create {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): Create =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        Create(
          on = path,
          credentials,
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
    credentials: Credentials,
    permission: Option[Int] = None) extends Action

  object Mkdirs {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): Mkdirs =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        Mkdirs(
          on = path,
          credentials,
          permission = queryParamOpt(request, "permission").map(Integer.parseInt(_, 8))
        )
      }
  }

  /** A RENAME action on a Infinity path. */
  case class Rename(on: Path, credentials: Credentials, destination: String) extends Action

  object Rename {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): Rename =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        Rename(
          on = path,
          credentials,
          destination = queryParam(request, "destination")
        )
      }
  }

  /** A SETREPLICATION action on a Infinity path. */
  case class SetReplication(
    on: Path,
    credentials: Credentials,
    replication: Option[Short] = None) extends Action

  object SetReplication {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): SetReplication =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        SetReplication(
          on = path,
          credentials,
          replication = queryParamOpt(request, "replication").map(_.toShort)
        )
      }
  }

  /** A SETOWNER action on a Infinity path. */
  case class SetOwner(
    on: Path,
    credentials: Credentials,
    owner: Option[String] = None,
    group: Option[String] = None) extends Action

  object SetOwner {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): SetOwner =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        SetOwner(
          on = path,
          credentials,
          owner = queryParamOpt(request, "owner"),
          group = queryParamOpt(request, "group")
        )
      }
  }

  /** A SETPERMISSION action on a Infinity path. */
  case class SetPermission(
    on: Path,
    credentials: Credentials,
    permission: Option[Int] = None) extends Action

  object SetPermission {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): SetPermission =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        SetPermission(
          on = path,
          credentials,
          permission = queryParamOpt(request, "permission").map(Integer.parseInt(_, 8))
        )
      }
  }

  /** A SETTIMES action on a Infinity path. */
  case class SetTimes(
    on: Path,
    credentials: Credentials,
    modificationTime: Option[Long] = None,
    accessTime: Option[Long] = None) extends Action

  object SetTimes {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): SetTimes =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        SetTimes(
          on = path,
          credentials,
          modificationTime = queryParamOpt(request, "modificationtime").map(_.toLong),
          accessTime = queryParamOpt(request, "accesstime").map(_.toLong)
        )
      }
  }

  /** A RENEWDELEGATIONTOKEN action on a Infinity path. */
  case class RenewDelegationToken(
    on: Path, credentials: Credentials, token: String) extends Action

  object RenewDelegationToken {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): RenewDelegationToken =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        RenewDelegationToken(
          on = path,
          credentials,
          token = queryParam(request, "token")
        )
      }
  }

  /** A CANCELDELEGATIONTOKEN action on a Infinity path. */
  case class CancelDelegationToken(
    on: Path, credentials: Credentials, token: String) extends Action

  object CancelDelegationToken {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): CancelDelegationToken =
      withPutBasicProps(remoteAddress, request) { (path, credentials) =>
        CancelDelegationToken(
          on = path,
          credentials,
          token = queryParam(request, "token")
        )
      }
  }

  /** A APPEND action on a Infinity path. */
  case class Append(
    on: Path,
    credentials: Credentials,
    buffersize: Option[Int] = None) extends Action

  object Append {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): Append =
      withPostBasicProps(remoteAddress, request) { (path, credentials) =>
        Append(
          on = path,
          credentials,
          buffersize = queryParamOpt(request, "buffersize").map(_.toInt)
        )
      }
  }

  /** A DELETE action on a Infinity path. */
  case class Delete(
    on: Path,
    credentials: Credentials,
    recursive: Option[Boolean] = None) extends Action

  object Delete {

    def apply(remoteAddress: InetSocketAddress, request: HttpRequest): Delete =
      withDeleteBasicProps(remoteAddress, request) { (path, credentials) =>
        Delete(
          on = path,
          credentials,
          recursive = queryParamOpt(request, "recursive").map(_.toBoolean)
        )
      }
  }
}
