/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

import es.tid.cosmos.common.{BearerToken, Wrapped}

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
      case Wrapped(StatusCode(503)) =>
        log.error("Infinity API is temporarily unavailable. " +
          "Check if the service is running or starting.")
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

  private val headers = Map("Authorization" -> BearerToken(infinitySecret))

  private def requestGroups(handle: String) =
    Http(resource() <<? Map("handle" -> handle) <:< headers OK as.String)

  private def resource(): RequestBuilder = url(apiBase) / "infinity" / "v1" / "groups"
}
