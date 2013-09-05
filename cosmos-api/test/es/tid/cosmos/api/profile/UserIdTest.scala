package es.tid.cosmos.api.profile

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class UserIdTest extends FlatSpec with MustMatchers {

  "A valid user identifier" must "have a non-empty realm" in {
    evaluating { UserId("", "user") } must produce [IllegalArgumentException]
  }

  it must "have a non-empty id" in {
    evaluating { UserId("realm", "") } must produce [IllegalArgumentException]
  }

  it must "have the 'default' realm by default" in {
    UserId("user").realm must be ("default")
  }

  it must "be printed as user@realm" in {
    UserId("realm", "user").toString must be ("user@realm")
  }
}
