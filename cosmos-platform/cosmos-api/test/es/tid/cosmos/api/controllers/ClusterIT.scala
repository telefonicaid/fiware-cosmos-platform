package es.tid.cosmos.api.controllers

import scala.Some

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import play.api.test.Helpers._
import play.api.test.{FakeRequest, FakeApplication}
import play.api.libs.json.Json

import es.tid.cosmos.api.Global
import es.tid.cosmos.api.sm.MockedServiceManager
import es.tid.cosmos.servicemanager.ClusterId

/**
 * @author sortega
 */
class ClusterIT extends FlatSpec with MustMatchers {
  val resourcePath = s"/cosmos/cluster/${MockedServiceManager.defaultClusterId.uuid.toString}"

  "Cluster resource" must "list cluster details on GET request" in {
    running(FakeApplication(withGlobal = Some(Global))) {
      val resource = route(FakeRequest(GET, resourcePath)).get
      status(resource) must equal (OK)
      contentType(resource) must be (Some("application/json"))
      val description = Json.parse(contentAsString(resource))
      (description \ "id").as[String] must equal (MockedServiceManager.defaultClusterId.uuid.toString)
    }
  }

  it must "return 404 on unknown cluster" in {
    val unknownClusterId = ClusterId()
    running(FakeApplication(withGlobal = Some(Global))) {
      val resource = route(FakeRequest(GET, s"/cosmos/cluster/${unknownClusterId}")).get
      status(resource) must equal (NOT_FOUND)
    }
  }
}
