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

package es.tid.comos.infinity.client

import java.io.{InputStreamReader, OutputStreamWriter}
import java.net.{ConnectException, URL}
import scala.concurrent.Future
import scala.concurrent.ExecutionContext.Implicits.global

import com.ning.http.client.{RequestBuilder, Response}
import dispatch.{Future => _, _}

import es.tid.cosmos.common.Wrapped
import es.tid.cosmos.infinity.common.{Path, RootPath, SubPath}
import es.tid.cosmos.infinity.common.messages.{Action, ErrorDescriptor, PathMetadata}
import es.tid.cosmos.infinity.common.messages.json._
import es.tid.cosmos.infinity.common.permissions.PermissionsMask
import es.tid.cosmos.infinity.common.messages.Action.{CreateDirectory, CreateFile}

class HttpInfinityClient(metadataEndpoint: URL) extends InfinityClient {

  private val metadataParser = new MetadataParser()
  private val errorParser = new ErrorDescriptorParser()
  private val actionFormatter = new ActionMessageFormatter()

  override def pathMetadata(path: Path): Future[Option[PathMetadata]] =
    httpRequest(metadataResource(path)) { response =>
      response.getStatusCode match {
        case 200 => Some(metadataParser.parse(response.getResponseBody))
        case 404 => None
      }
    }

  override def createFile(
      path: SubPath, permissions: PermissionsMask,
      replication: Option[Int],
      blockSize: Option[Int]): Future[Unit] =
    createPath(path, CreateFile(path.name, permissions, replication, blockSize))

  override def createDirectory(path: SubPath, permissions: PermissionsMask): Future[Unit] =
    createPath(path, CreateDirectory(path.name, permissions))

  private def createPath(path: SubPath, action: Action): Future[Unit] = {
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

  /** Move a file or directory.
    *
    * The movement should not create a loop (i.e. moving a directory to a subdirectory).
    *
    * @param originPath  Path to move
    * @param targetPath  Destination path that should exist as a directory
    * @return            A successful future if the path is moved. Otherwise the common exceptions
    *                    or AlreadyExistsException when the target path already exists
    */
  override def move(originPath: Path, targetPath: Path): Future[Unit] = ???

  /** Change path group.
    *
    * @param path   Path to modify
    * @param group  New group
    * @return       Success or failure as a future
    */
  override def changeGroup(path: Path, group: String): Future[Unit] = ???

  /** Change path owner.
    *
    * @param path   Path to modify
    * @param owner  New owner
    * @return       Success or failure as a future
    */
  override def changeOwner(path: Path, owner: String): Future[Unit] = ???

  /** Change path permissions mask.
    *
    * @param path  Path to modify
    * @param mask  New mask
    * @return      Success or failure as a future
    */
  override def changeOwner(path: Path, mask: PermissionsMask): Future[Unit] = ???

  /** Overwrite a file.
    *
    * @param path  Path of the file to be overwritten
    * @return      A stream to write to, fail with the common exceptions or with NotFoundException
    *              when the path to write doesn't exist. Note that the returned stream can fail
    *              wrapping in IOException any common exception
    */
  override def overwrite(path: Path): Future[OutputStreamWriter] = ???

  /** Append data to a file.
    *
    * @param path  Path of the file to append to
    * @return      A stream to write to, fail with the common exceptions or with NotFoundException
    *              when the path to write doesn't exist. Note that the returned stream can fail
    *              wrapping in IOException any common exception
    */
  override def append(path: Path): Future[OutputStreamWriter] = ???

  /** Delete a file or directory.
    *
    * @param path  Path to delete
    * @return      Success or failure as a future. Apart from the common exceptions, fail with
    *              NotFoundException when the path to delete doesn't exist
    */
  override def delete(path: Path): Future[Unit] = ???

  /** Retrieve file contents.
    *
    * @param path    Path of the file to read
    * @param offset  Optionally, where to start reading
    * @param length  Optionally, how much to retrieve
    * @return        A stream with the contents on success or fail with the common exceptions or
    *                NotFoundException when the path to read doesn't exist
    */
  override def read(path: Path, offset: Option[Long], length: Option[Long]): Future[InputStreamReader] = ???
}
