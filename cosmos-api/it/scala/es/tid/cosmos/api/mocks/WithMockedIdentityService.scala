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

package es.tid.cosmos.api.mocks

import org.specs2.execute.{Result, AsResult}

class WithMockedIdentityService(val identityService: IdentityService = new IdentityService())
  extends WithSampleUsers(Map(
    "tuid.auth.url" -> identityService.baseUrl,
    "tuid.api.url" -> identityService.baseUrl,
    "tuid.client.id" -> identityService.clientId,
    "tuid.client.secret" -> identityService.clientSecret
  )) {

  override def around[T: AsResult](t: => T): Result = {
    super.around {
      identityService.start()
      try {
        t
      } finally {
        identityService.stop()
      }
    }
  }
}
