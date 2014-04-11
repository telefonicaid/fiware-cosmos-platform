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

package es.tid.cosmos.api.controllers.infinity

import scalaz.Scalaz

import com.typesafe.config.Config
import com.wordnik.swagger.annotations._
import play.api.libs.json.Json
import play.api.mvc.{Action, Controller, Results}

import es.tid.cosmos.api.controllers.common._
import es.tid.cosmos.api.profile.CosmosProfile
import es.tid.cosmos.api.profile.dao.ProfileDataStore
import es.tid.cosmos.api.quota.{GuaranteedGroup, NoGroup}

@Api(value = "/infinity/v1/groups", listingPath = "/doc/infinity/v1/groups",
  description = "Maps user handles to groups for Infinity")
class GroupMapperResource(store: ProfileDataStore, config: Config)
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
  def map(@ApiParam(name = "User handle") handle: Option[String]) =
    Action { implicit request =>
      for {
        _ <- requestAuthentication.requireAuthorized(request)
        userProfile <- requireUserWithHandle(handle)
      } yield Ok(Json.toJson(groupsFor(userProfile)))
    }

  private def requireUserWithHandle(handleOpt: Option[String]): ActionValidation[CosmosProfile] = for {
    handle <- handleOpt.toSuccess(MissingHandleResponse)
    profile <- store.withTransaction {implicit c =>
      store.profile.lookupByHandle (handle)
    }.toSuccess(invalidHandleResponse(handle))
  } yield profile

  private def invalidHandleResponse(handle: String) =
    BadRequest(Json.toJson(Message(s"Handle $handle is invalid")))

  private def groupsFor(profile: CosmosProfile) = {
    val group = profile.group match {
      case NoGroup => "cosmos"
      case GuaranteedGroup(name, _) => name
    }
    UserGroups(Seq(group))
  }
}

private object GroupMapperResource extends Results {
  val MissingHandleResponse = BadRequest(Json.toJson(Message("Missing handle parameter")))
}
