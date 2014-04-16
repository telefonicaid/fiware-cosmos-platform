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
