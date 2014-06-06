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

package es.tid.cosmos.api.controllers.infinity

import scalaz.Scalaz

import com.typesafe.config.Config
import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Action, Controller, Results}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.CosmosProfile
import es.tid.cosmos.api.profile.dao.{GroupDataStore, ProfileDataStore}
import es.tid.cosmos.api.quota.{GuaranteedGroup, NoGroup}

@Api(value = "/infinity/v1/groups", listingPath = "/doc/infinity/v1/groups",
  description = "Maps user handles to groups for Infinity")
class GroupMapperResource(store: ProfileDataStore with GroupDataStore, config: Config)
  extends Controller {

  import Scalaz._
  import GroupMapperResource._

  private val requestAuthentication = new InfinityRequestAuthentication(config)

  @ApiOperation(value = "Map a user handle to its groups", httpMethod = "GET",
    responseClass = "es.tid.cosmos.api.controllers.infinity.UserGroups")
  @ApiErrors(Array(
    new ApiError(code = 400, reason = "For missing handles"),
    new ApiError(code = 400, reason = "If the handle corresponds to no profile"),
    new ApiError(code = 401, reason = "Request lacks a basic authorization header"),
    new ApiError(code = 401, reason = "Invalid authentication credentials")
  ))
  def map(@ApiParam(name = "User handle") maybeHandle: Option[String]) =
    Action { implicit request =>
      def handleResult(handle: String) = requireUserWithHandle(handle).map(groupsFor)
      def groupResult(handle: String) = requireGroupHandle(handle).map(toUserGroups)
      for {
        _ <- requestAuthentication.requireAuthorized(request)
        handle <- maybeHandle.toSuccess(MissingHandleResponse)
        groups <- handleResult(handle).orElse(groupResult(handle)).toSuccess(
          invalidHandleResponse(handle))
      } yield Ok(Json.toJson(groups))
    }

  private def requireGroupHandle(groupName: String): Option[GuaranteedGroup] =
    store.withTransaction { implicit c =>
      store.group.lookupByName(groupName)
    }

  private def requireUserWithHandle(handle: String): Option[CosmosProfile] =
    store.withTransaction {implicit c =>
      store.profile.lookupByHandle (handle)
    }

  private def invalidHandleResponse(handle: String) =
    BadRequest(Json.toJson(Message(s"Handle $handle is invalid")))

  private def groupsFor(profile: CosmosProfile) = {
    val group = profile.group match {
      case NoGroup => "cosmos"
      case GuaranteedGroup(name, _) => name
    }
    UserGroups(Seq(group))
  }

  private def toUserGroups(group: GuaranteedGroup) = {
    UserGroups(Seq(group.name))
  }
}

private object GroupMapperResource extends Results {
  val MissingHandleResponse = BadRequest(Json.toJson(Message("Missing handle parameter")))
}
