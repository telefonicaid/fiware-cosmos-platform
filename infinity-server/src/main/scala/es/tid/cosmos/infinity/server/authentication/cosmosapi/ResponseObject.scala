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

package es.tid.cosmos.infinity.server.authentication.cosmosapi

import java.nio.charset.Charset
import scala.util.{Failure, Success, Try}

import com.twitter.finagle.http.Response
import net.liftweb.json._
import net.liftweb.json.JsonParser.ParseException

import es.tid.cosmos.infinity.server.authentication.UserProfile
import es.tid.cosmos.infinity.server.permissions.PermissionsMask

private[cosmosapi] case class ResponseObject(
    user: String,
    group: String,
    accessMask: String,
    origins: Option[Seq[String]]) {

  require(PermissionsMask.isValidOctal(accessMask),
    s"invalid access mask expression '$accessMask': octal value was expected")

  def toUserProfile = UserProfile(
    username = user,
    group = group,
    mask = PermissionsMask.fromOctal(accessMask)
  )
}

object ResponseObject {

  private implicit val formats = DefaultFormats

  def extractFrom(response: Response): Try[ResponseObject] = {
    val body: String = response.content.toString(Charset.defaultCharset())
    try {
      Success(parse(body).extract[ResponseObject])
    } catch {
      case e: MappingException =>
        Failure(new IllegalArgumentException(
          s"cannot map response body to expected object: $body", e))
      case e: ParseException =>
        Failure(new IllegalArgumentException(
          s"cannot parse response body to a JSON object: $body", e))
    }
  }
}
