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

package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.api.profile.UserState._

class UserStateTest extends FlatSpec with MustMatchers {

  "A user state" must "be convertible to string" in {
    UserState.values.map(_.toString).toList.sorted must equal (Seq(
      "deleted", "deleting", "disabled", "enabled"))
  }

  it must "be extracted from its string representation" in {
    UserState.unapply("disabled") must be (Some(Disabled))
  }

  it must "be extracted in case-insensitive fashion" in {
    UserState.unapply("DeleTed") must be (Some(Deleted))
  }

  it must "not be extracted from unknown strings" in {
    UserState.unapply("unknown") must not be 'defined
  }
}
