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

package es.tid.cosmos.tests.e2e

import dispatch.{Future => _, _}
import dispatch.Defaults._

class ExternallyManagedUsersIT extends E2ETestBase {
  feature("An authorized entity is able to manage external users") {
    var user: User = null

    scenario("The authorized entity can create users") {
      user = new User()
      val response_> = Http(user.profileResource > as.String)
      response_> must (runUnder(restTimeout) and eventually(include(user.handle)))
    }

    scenario("The authorized entity can delete users") {
      user.delete()
    }
  }
}
