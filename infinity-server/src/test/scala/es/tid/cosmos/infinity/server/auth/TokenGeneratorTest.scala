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

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import spray.http.Uri

class TokenGeneratorTest extends FlatSpec with MustMatchers {

  /*
   * export PHRASE="segredo1324527723/p/files/top_secret.pdf"
   * echo -n "$PHRASE" | openssl md5 -binary | openssl base64 | tr +/ -_ | tr -d =
   */
  "Token generator" must "encode URI and timestamp without query params" in {
    // Sample values obtained from http://wiki.nginx.org/HttpSecureLinkModule
    TokenGenerator("segredo").encode(
      uri = Uri("http://localhost/p/files/top_secret.pdf"),
      expireInstant = 1324527723
    ) must be ("ZTKCsvSKaJgSCoUVi8CkXQ")
  }

  /*
   * export PHRASE="segredo1324527723/p/files/top_secret.pdf?permission=755&op=OPEN"
   * echo -n "$PHRASE" | openssl md5 -binary | openssl base64 | tr +/ -_ | tr -d =
   */
  it must "encode URI and timestamp with query params" in {
    // Sample values obtained from http://wiki.nginx.org/HttpSecureLinkModule
    TokenGenerator("segredo").encode(
      uri = Uri("http://localhost/p/files/top_secret.pdf?permission=755&op=OPEN"),
      expireInstant = 1324527723
    ) must be ("5VacXTgihXb3hJ56aS_TKg")
  }

  /*
   * export PHRASE="segredo until 1324527723 with path /p/files/top_secret.pdf?permission=755&op=OPEN"
   * echo -n "$PHRASE" | openssl md5 -binary | openssl base64 | tr +/ -_ | tr -d =
   */
  it must "encode URI and timestamp using custom template" in {
    // Sample values obtained from http://wiki.nginx.org/HttpSecureLinkModule
    TokenGenerator("segredo", "${secret} until ${expire} with path ${path}${query}").encode(
      uri = Uri("http://localhost/p/files/top_secret.pdf?permission=755&op=OPEN"),
      expireInstant = 1324527723
    ) must be ("F8chPMICSnrR9mMsXqxpdA")
  }
}
