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

package es.tid.cosmos.api.auth.oauth2

import play.core.parsers.FormUrlEncodedParser

import es.tid.cosmos.api.auth.oauth2.OAuthError._

/**
 * Represents an OAuth 2.0 exception.
 *
 * @param error        Error type
 * @param description  Optional human readable description from the provider
 */
case class OAuthException(error: OAuthError, description: String)
  extends Exception(s"OAuth 2.0 error: $error")

object OAuthException {

  /**
   * Creates the exception from a url-encoded text as in RFC 6749 section 5.2
   *
   * @param urlEncodedError  encoded form
   * @return                 OAuthException or None
   */
  def fromForm(urlEncodedError: String): Option[OAuthException] = {
    val form = FormUrlEncodedParser.parse(urlEncodedError, "UTF-8")
    for {
      value <- firstValue("error", form)
      error <- OAuthError.parse(value)
    } yield OAuthException(error, firstValue("error_description", form).getOrElse("unknown error"))
  }

  private def firstValue(field: String, form: Map[String, Seq[String]]): Option[String] =
    for (values <- form.get(field); firstValue <- values.headOption) yield firstValue
}
