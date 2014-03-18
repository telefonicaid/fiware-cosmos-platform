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

package es.tid.cosmos.infinity.server.auth

import org.apache.commons.codec.digest.DigestUtils
import org.apache.commons.codec.binary.Base64
import spray.http.Uri

import es.tid.cosmos.infinity.server.config.AuthTokenConfig

/** A generator of secure link tokens used by Infinity Server.
  *
  * Secure link tokens are used by Infinity Server to sign the redirection of the incoming requests
  * to the data nodes. The token is comprised by an base64 encoded MD5 resume of a phrase that can
  * be customized. By default, the phrase is the concatenation of the following fields:
  * <ul>
  *   <li>The secret shared by Infinity Server and datanode web proxies.
  *   <li>The instant when the token expires, represented as seconds from the Epoch.
  *   <li>The URI path
  *   <li>The URI query params, including leading '?' if there are one or more params.
  * </ul>
  *
  * @param secret the secret shared by Infinity Server and datanode web proxies
  * @param phraseTemplate the template used to generate the phrase to encode. The following
  *                       substitution words can be used.
  *                       <ul>
  *                         <li>${secret}, which will be replaced by the shared secret
  *                         <li>${expire}, which will be replaced by expire instant
  *                         <li>${path}, which will be replaced by URI path (w/o query params)
  *                         <li>${query}, which will be replaced by URI query params
  *                       </ul>
  */
class TokenGenerator(secret: String, phraseTemplate: String) {

  private val md5 = DigestUtils.getMd5Digest

  /** Create a new token from the given URI, valid until given instant.
    *
    * @param uri the URI used to generate the token
    * @param expireInstant the instant when the token will be no longer valid
    * @return the newly generated token
    */
  def encode(uri: Uri, expireInstant: Long): String = {
    val uriPath = uri.path.toString()
    val uriQuery = {
      val query = uri.query.toString()
      if (query.isEmpty) query else s"?$query"
    }
    val phrase = phraseTemplate
      .replace("${secret}", secret)
      .replace("${expire}", expireInstant.toString)
      .replace("${path}", uriPath)
      .replace("${query}", uriQuery)
    Base64.encodeBase64URLSafeString(md5.digest(phrase.getBytes))
  }
}

object TokenGenerator {

  /** Create a new token generator from the given shared secret. */
  def apply(
      secret: String,
      phraseTemplate: String = AuthTokenConfig.DefaultPhraseTemplate): TokenGenerator =
    new TokenGenerator(secret, phraseTemplate)

  /** Create a new token generator from the auth token configuration. */
  def apply(config: AuthTokenConfig): TokenGenerator =
    TokenGenerator(config.secret, config.phraseTemplate)
}
