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

package es.tid.cosmos.api.oauth2

import play.core.parsers.FormUrlEncodedParser

import es.tid.cosmos.api.oauth2.OAuthError._

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
