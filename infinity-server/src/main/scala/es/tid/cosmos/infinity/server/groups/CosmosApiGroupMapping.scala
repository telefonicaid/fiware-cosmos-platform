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

package es.tid.cosmos.infinity.server.groups

import java.util.concurrent.TimeoutException
import scala.collection.JavaConverters._
import scala.concurrent.{Await, Future}
import scala.concurrent.duration.Duration
import scala.util.{Failure, Success}

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}
import dispatch.Defaults._
import org.apache.commons.logging.LogFactory
import org.apache.hadoop.security.GroupMappingServiceProvider

import es.tid.cosmos.common.Wrapped

/** Client of the Cosmos API able to retrieve the groups of a given user identified by handle.
  *
  * @constructor
  * @param apiBase  Base URL of the API of the form https://host:port/
  *                 (without the suffix /infinity/v1/)
  * @param infinitySecret  Secret shared between infinity server and Cosmos API for request
  *                        authentication
  * @param apiTimeout  Time to wait for server responses
  * @param fallbackMapping  Mapping to use for users the server don't know (like the hdfs user)
  */
private[groups] class CosmosApiGroupMapping(
    apiBase: String,
    infinitySecret: String,
    apiTimeout: Duration,
    fallbackMapping: GroupMappingServiceProvider) extends GroupMapping {

  private val log = LogFactory.getLog(classOf[CosmosApiGroupMapping])
  log.debug(s"Mapping user to groups against Cosmos API at $apiBase with $apiTimeout timeout")

  /** @inheritdoc
    *
    * Several things can go wrong when querying the Cosmos API. In that cases, it is
    * better to return an empty list and log the error on detail to avoid making unstable the
    * Hadoop daemons.
    */
  override def groupsFor(handle: String): GroupList = {
    val groups = blockForResult(queryService(handle))
    log.debug(s"Groups for $handle: " + groups.mkString("[", ", ", "]"))
    groups
  }

  private def queryService(handle: String): Future[GroupList] =
    requestGroups(handle).map(UserGroups.parse).recover {
      case Wrapped(StatusCode(400)) =>
        log.info(s"Cosmos API doesn't recognize '$handle' as an existing user. " +
          s"Falling back to $fallbackMapping")
        fallbackMapping.getGroups(handle).asScala
      case Wrapped(StatusCode(403)) =>
        log.error("Infinity secret was rejected. " +
          "Please check that infinity server and cosmos-api configurations match.")
        Seq.empty
      case Wrapped(ex @ StatusCode(_)) =>
        log.error(s"Unexpected Cosmos API response when mapping groups of $handle", ex)
        Seq.empty
    }

  private def blockForResult(task: Future[GroupList]): GroupList = try {
    Await.ready(task, apiTimeout).value.get match {
      case Success(result) => result
      case Failure(unexpectedException) =>
        log.error("Unexpected error mapping user to groups", unexpectedException)
        Seq.empty
    }
  } catch {
    case ex: TimeoutException =>
      log.error(s"Cosmos API is taking more than $apiTimeout when mapping users to groups", ex)
      Seq.empty
  }

  private def requestGroups(handle: String) =
    Http(resource() <<? Map("handle" -> handle) as_! ("infinity", infinitySecret) OK as.String)

  private def resource(): RequestBuilder = url(apiBase) / "infinity" / "v1" / "groups"
}
