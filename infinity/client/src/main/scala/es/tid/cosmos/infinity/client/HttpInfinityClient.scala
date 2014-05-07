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

package es.tid.cosmos.infinity.client

import java.io._
import java.net.{ConnectException, URL}
import scala.concurrent.{Future, blocking}
import scala.concurrent.ExecutionContext.Implicits.global

import com.ning.http.client.{RequestBuilder, Response}
import com.ning.http.client.generators.InputStreamBodyGenerator
import dispatch.{Future => _, _}

import es.tid.cosmos.common.Wrapped
import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.infinity.common.json._
import es.tid.cosmos.infinity.common.messages._
import es.tid.cosmos.infinity.common.messages.Request._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask

class HttpInfinityClient(metadataEndpoint: URL) extends InfinityClient {

  private val metadataParser = new MetadataParser()
  private val errorParser = new ErrorDescriptorParser()
  private val actionFormatter = new RequestMessageFormatter()

  override def pathMetadata(path: Path): Future[Option[PathMetadata]] =
    httpRequest(metadataResource(path)) { response =>
      response.getStatusCode match {
        case 200 => Some(metadataParser.parse(response.getResponseBody))
        case 404 => None
      }
    }

  override def createFile(
      path: SubPath,
      permissions: PermissionsMask,
      replication: Option[Short],
      blockSize: Option[Long]): Future[Unit] =
    createPath(path, CreateFile(path.name, permissions, replication, blockSize))

  override def createDirectory(path: SubPath, permissions: PermissionsMask): Future[Unit] =
    createPath(path, CreateDirectory(path.name, permissions))

  override def move(originPath: SubPath, targetPath: Path): Future[Unit] = {
    val body = actionFormatter.format(Move(originPath.name, originPath.parentPath))
    httpRequest(metadataResource(targetPath) << body) { response =>
      response.getStatusCode match {
        case 404 => throw NotFoundException(originPath)
        case 409 => throw AlreadyExistsException(targetPath / originPath.name)
        case 201 => ()
      }
    }
  }

  override def changeOwner(path: Path, owner: String): Future[Unit] =
    actionWithNoContentResponse(path, ChangeOwner(owner))

  override def changeGroup(path: Path, group: String): Future[Unit] =
    actionWithNoContentResponse(path, ChangeGroup(group))

  override def changePermissions(path: Path, mask: PermissionsMask): Future[Unit] =
    actionWithNoContentResponse(path, ChangePermissions(mask))

  override def delete(path: SubPath, isRecursive: Boolean = false): Future[Unit] = {
    val params = Map("recursive" -> isRecursive.toString)
    requestWithNoContentResponse(path, (metadataResource(path) <<? params).DELETE)
  }

  override def read(
      path: SubPath, offset: Option[Long], length: Option[Long]): Future[InputStream] =
    // read metadata first, get content url and then read content
    existingMetaData(path) flatMap { metadata =>
      val params = List(
        offset.map("offset" -> _.toString),
        length.map("length" -> _.toString)
      ).flatten.toMap

      httpRequest(contentResource(metadata) <<? params) { response =>
        response.getStatusCode match {
          case 404 => throw NotFoundException(path)
          case 200 => response.getResponseBodyAsStream
        }
      }
    }

  override def overwrite(path: SubPath, bufferSize: Int): Future[OutputStream] =
    requestWithOutputStream(path, _.PUT, bufferSize)

  override def append(path: SubPath, bufferSize: Int): Future[OutputStream] =
    requestWithOutputStream(path, _.POST, bufferSize)

  private def requestWithOutputStream(
      path: Path,
      requestMethod: RequestBuilder => RequestBuilder,
      bufferSize: Int): Future[OutputStream] =
    existingMetaData(path) map { metadata =>
      /* Create a stream pipes to allow the caller to write on the output stream
       * while a separate thread is reading its input stream to form the HTTP request body
       */
      val in = new PipedInputStream(bufferSize)
      val out = new PipedOutputStream(in)
      val request = requestMethod(contentResource(metadata)).setBody(
        new InputStreamBodyGenerator(in))
      /* While the request is async, ning blocks to acquire the request's body.
       * Since the body comes from an async input stream we need to wrap the request in another
       * async block.
       */
      Future { blocking { httpRequest(request) { response =>
        response.getStatusCode match {
          case 404 => throw NotFoundException(path)
          case 204 => ()
        }
      }}}
      out
    }


  private def existingMetaData(path: Path): Future[PathMetadata] =
    pathMetadata(path) map (_.getOrElse(throw NotFoundException(path)))

  private def createPath(path: SubPath, action: Request): Future[Unit] = {
    val body = actionFormatter.format(action)
    httpRequest(metadataResource(path.parentPath) << body) { response =>
      response.getStatusCode match {
        case 404 => throw NotFoundException(path.parentPath)
        case 409 => throw AlreadyExistsException(path)
        case 201 => ()
      }
    }
  }

  /** Perform a metadata request calling `handler` only if no common error has happened. */
  private def httpRequest[T](request: RequestBuilder)(handler: Response => T): Future[T] =
    translateExceptions(Http(request > withCommonErrorHandling(handler)))

  private def withCommonErrorHandling[T](handler: Response => T): Response => T =
    (handleCommonErrors _).andThen(handler)

  private def handleCommonErrors(response: Response): Response = response.getStatusCode match {
    case 400 =>
      val error = parseError(response)
      throw ProtocolMismatchException(error.cause, Some(error.code))
    case 403 =>
      throw ForbiddenException(parseError(response))
    case _ => response
  }

  /** Translate exceptions, mostly undoing the wrapping that Dispatch does. */
  private def translateExceptions[T](response: Future[T]): Future[T] = response.recoverWith {
    case Wrapped(ex: ConnectException) => Future.failed(ConnectionException(metadataEndpoint, ex))
    case Wrapped(Wrapped(ex: InfinityException)) => Future.failed(ex)
    case Wrapped(Wrapped(ex: ParseException)) =>
      Future.failed(ProtocolMismatchException("cannot parse server response", cause = ex))
  }

  private def parseError(response: Response): ErrorDescriptor =
    errorParser.parse(response.getResponseBody)

  private def metadataResource(path: Path): RequestBuilder = path match {
    case RootPath => metadataRequestBuilder()
    case SubPath(parentPath, name) => metadataResource(parentPath) / name
  }

  private def metadataRequestBuilder(): RequestBuilder =
    url(metadataEndpoint.toString) / "infinityfs" / "v1" / "metadata"

  private def contentResource(metadata: PathMetadata): RequestBuilder = metadata match {
    case f: FileMetadata =>
      val location = f.content.getOrElse(throw NotFoundException(metadata.path))
      url(location.toString)
    case d: DirectoryMetadata => throw new IllegalArgumentException("Directory cannot have content")
  }

  private def actionWithNoContentResponse(path: Path, action: Request): Future[Unit] = {
    val body = actionFormatter.format(action)
    requestWithNoContentResponse(path, metadataResource(path) << body)
  }

  private def requestWithNoContentResponse(path: Path, request: RequestBuilder): Future[Unit] =
    httpRequest(request) { response =>
      response.getStatusCode match {
        case 404 => throw NotFoundException(path)
        case 204 => ()
      }
    }
}
