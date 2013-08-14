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

package es.tid.cosmos.api.mocks.oauth2

import scala.util.Random

import org.specs2.execute.{Result, AsResult}

import es.tid.cosmos.api.mocks.WithSampleUsers

class WithMockedTuIdService(
    port: Int = WithMockedTuIdService.randomPort(),
    clientId: String = "fake client id",
    clientSecret: String = "fake client secret"
  ) extends WithSampleUsers(additionalConfiguration = Map(
    "oauth.client.id" -> clientId,
    "oauth.client.secret" -> clientSecret,
    "tuid.auth.url" -> s"http://127.0.0.1:$port",
    "tuid.api.url" -> s"http://127.0.0.1:$port"
  )) {

  lazy val tuId: TuIdService = new TuIdService(port, clientId, clientSecret)

  override def around[T: AsResult](t: => T): Result = {
    super.around {
      tuId.start()
      try {
        t
      } finally {
        tuId.stop()
      }
    }
  }
}

object WithMockedTuIdService {
  def randomPort() = Random.nextInt(5000) + 1000
}
