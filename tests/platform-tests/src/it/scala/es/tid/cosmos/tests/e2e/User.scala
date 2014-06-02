/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.tests.e2e

import java.io.Closeable
import java.nio.charset.Charset
import java.nio.file.{Files, StandardOpenOption}
import scala.collection.JavaConversions._
import scala.concurrent.duration._
import scala.io.Source
import scala.language.postfixOps
import scala.util.{Try, Random}

import com.typesafe.config.Config
import dispatch.{Future => _, _}
import dispatch.Defaults._
import net.liftweb.json._
import net.liftweb.json.JsonDSL._
import org.scalatest.Informer
import org.scalatest.verb.MustVerb
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.matchers.FutureMatchers
import es.tid.cosmos.api.controllers.admin.RegisterUserResponse

class User(implicit info: Informer, testConfig: Config) extends Closeable
  with MustVerb with MustMatchers with Patience with FutureMatchers {

  implicit val formats = DefaultFormats

  private def cosmos = url(testConfig.getString("apiUrl"))
  private val realm = testConfig.getString("realm")
  private val realmSecret = testConfig.getString("realmSecret")
  private def userResource = (cosmos / "admin" / "v1" / "user").as_!(realm, realmSecret)
  private val restTimeout = testConfig.getInt("restTimeout").seconds

  val id = "testUser" + Random.nextInt(Int.MaxValue)
  val email = s"$id@$realm"
  val publicKey = {
    val source = Source.fromFile(System.getProperty("user.home") + "/.ssh/id_rsa.pub")
    val result = Try(source.mkString)
    source.close()
    result.get
  }

  private val registrationResponse = register()

  val apiKey = registrationResponse.apiKey
  val apiSecret = registrationResponse.apiSecret
  val handle = registrationResponse.handle
  def profileResource =
    (cosmos / "cosmos" / "v1" / "profile").as_!(apiKey, apiSecret)
  val cosmosrcPath = {
    val tempFile = Files.createTempFile(handle, "cosmosrc")
    val contents = Seq(s"""api_key: $apiKey
      |api_secret: $apiSecret
      |api_url: ${cosmos.url}/cosmos/v1/
      |ssh_command: ssh
      |ssh_key: ''""".stripMargin)

    Files.write(
      tempFile,
      contents,
      Charset.defaultCharset(),
      StandardOpenOption.WRITE
    )
    info(s"The cosmosrc path is ${tempFile.toAbsolutePath}")
    tempFile.toAbsolutePath
  }

  private def register() = {
    info(s"Starting registration for user with id $id")
    val request = userResource << compact(render(
      ("authId" -> id) ~
      ("authRealm" -> realm) ~
      ("email" -> email) ~
      ("sshPublicKey" -> publicKey)
    ))
    info(compact(render(
      ("authId" -> id) ~
        ("authRealm" -> realm) ~
        ("email" -> email) ~
        ("sshPublicKey" -> publicKey)
    )))
    val response_> = Http(request > as.String).map {
      responseBody => {
        parse(responseBody).extract[RegisterUserResponse]
      }
    }
    response_> must (runUnder(restTimeout) and eventuallySucceed)
    info(s"The user creation REST request returned 200")
    val response = response_>.value.get.get
    info(s"The username being created is ${response.handle}")
    val localUserResource =
      (cosmos/ "cosmos" / "v1" / "profile").as_!(response.apiKey, response.apiSecret)
    eventually {
      val statusCode_> = Http(localUserResource).map(_.getStatusCode)
      statusCode_> must (runUnder(restTimeout) and eventually(be(200)))
    }
    response
  }

  def delete() = {
    val unregistration = (userResource / realm / id).DELETE
    Http(unregistration > as.String) must (runUnder(restTimeout) and eventuallySucceed)
    info(s"Deletion for user $handle started")
    eventually {
      val statusCode_> = Http(profileResource).map(_.getStatusCode)
      statusCode_> must (runUnder(restTimeout) and eventually(be(401)))
      info(s"User $handle removed")
    }
  }

  override def close() = delete()
}
