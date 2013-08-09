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

import org.specs2.execute.{Result, AsResult}

import es.tid.cosmos.api.mocks.WithSampleUsers

class WithMockedTuIdService extends WithSampleUsers {

  lazy val tuId: TuIdService = new TuIdService()

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
