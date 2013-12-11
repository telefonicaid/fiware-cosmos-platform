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

import scala.concurrent.duration._

import com.typesafe.config.{ConfigFactory, Config}
import dispatch.{Future => _, _}, Defaults._
import net.liftweb.json._
import net.liftweb.json.JsonDSL._
import org.scalatest.FlatSpec
import org.scalatest.concurrent.{Eventually, IntegrationPatience}
import org.scalatest.matchers.MustMatchers
import org.scalatest.time.{Seconds, Minutes, Span}

import es.tid.cosmos.platform.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.platform.common.scalatest.tags.EndToEndTest

class ExternallyManagedUsersIT
  extends FlatSpec with MustMatchers with Eventually with FutureMatchers with IntegrationPatience {

  implicit override val patienceConfig = PatienceConfig(
    timeout = scaled(Span(10, Minutes)),
    interval = scaled(Span(5, Seconds))
  )

  lazy val testConfig: Config = ConfigFactory.load(getClass.getClassLoader, "test.conf")
  val testTimeout = 10.seconds
  val realm = "horizon"
  val realmSecret = "horizon!"
  val id = "external" + scala.util.Random.nextInt(10000)

  var handle: String = null
  var apiKey: String = null
  var apiSecret: String = null

  "A user managed externally" must "be registered" taggedAs EndToEndTest in {
    val email = s"$id@$realm"
    val request = userResource << compact(render(
      ("authId" -> id) ~
      ("authRealm" -> realm) ~
      ("email" -> email) ~
      ("sshPublicKey" -> s"ssh-rsa XXXXXXX $email")
    ))
    val response_> = Http(request OK as.String).map { responseBody =>
      for(
        JObject(child) <- parse(responseBody);
        JField(name, JString(value)) <- child
      ) {
        name match {
          case "handle" => handle = value
          case "apiKey" => apiKey = value
          case "apiSecret" => apiSecret = value
        }
      }
    }
    response_> must (runUnder(testTimeout) and eventuallySucceed)
  }

  it must "have valid credentials" taggedAs EndToEndTest in {
    val response_> = Http(profileResource OK as.String)
    response_> must (runUnder(testTimeout) and eventually(include(handle)))
  }

  it must "be unregistered" taggedAs EndToEndTest in {
    val unregistration = (userResource / realm / id).DELETE
    Http(unregistration OK as.String) must (runUnder(testTimeout) and eventuallySucceed)
    eventually {
      val statusCode_> = Http(profileResource).map(_.getStatusCode)
      statusCode_> must (runUnder(testTimeout) and eventually(be(401)))
    }
  }

  def andromeda = url(testConfig.getString("apiUrl"))
  def userResource = (andromeda / "admin" / "v1" / "user").as_!(realm, realmSecret)
  def profileResource = (andromeda / "cosmos" / "v1" / "profile").as_!(apiKey, apiSecret)
}
