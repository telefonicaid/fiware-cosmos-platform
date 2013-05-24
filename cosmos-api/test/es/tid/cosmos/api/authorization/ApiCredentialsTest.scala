package es.tid.cosmos.api.authorization

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class ApiCredentialsTest extends FlatSpec with MustMatchers {
  "Api credentials" must "be randomly created on demand" in {
    ApiCredentials.random must not be (ApiCredentials.random)
  }

  it must "preserve API id size invariant" in {
    val ex = evaluating {
      ApiCredentials("small", "justright,justright,justright,justright,")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("API identifier")
  }

  it must "preserve API secret size invariant" in {
    val ex = evaluating {
      ApiCredentials("justright,justright,justright,justright,", "small")
    } must produce [IllegalArgumentException]
    ex.getMessage must include ("API secret")
  }
}
