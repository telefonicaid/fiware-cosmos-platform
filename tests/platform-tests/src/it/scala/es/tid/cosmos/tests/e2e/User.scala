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

import java.nio.charset.Charset
import java.nio.file.{Files, StandardOpenOption}
import java.nio.file.attribute.PosixFilePermissions
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

class User(implicit info: Informer, testConfig: Config) extends MustVerb
  with MustMatchers with Patience with FutureMatchers {

  implicit val formats = DefaultFormats

  private def cosmos = url(testConfig.getString("apiUrl"))
  private val realm = testConfig.getString("realm")
  private val realmSecret = testConfig.getString("realmSecret")
  private def userResource = (cosmos / "admin" / "v1" / "user").as_!(realm, realmSecret)
  private val restTimeout = testConfig.getInt("restTimeout").seconds

  val id = "testUser" + Random.nextInt(Int.MaxValue)
  val email = s"$id@$realm"
  lazy val publicKey = {
    val source = Source.fromFile(System.getProperty("user.home") + "/.ssh/id_rsa.pub")
    val result = Try(source.mkString)
    source.close()
    result.get
  }

  private var userCreated = false
  private lazy val registrationResponse = register()

  lazy val apiKey = registrationResponse.apiKey
  lazy val apiSecret = registrationResponse.apiSecret
  lazy val handle = registrationResponse.handle
  def profileResource =
    (cosmos / "cosmos" / "v1" / "profile").as_!(apiKey, apiSecret)
  lazy val cosmosrcPath = {
    val permissions = PosixFilePermissions.asFileAttribute(PosixFilePermissions.fromString("rw-------"))
    val tempFile = Files.createTempFile(handle, "cosmosrc", permissions)
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
    userCreated = true
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
    Thread.sleep((5 minutes).toMillis) // FIXME: Wait while user is being added to Infinity, etc.
    val response = response_>.value.get.get
    info(s"The username being created is ${response.handle}")
    response
  }

  def delete() = {
    if (userCreated) {
      val unregistration = (userResource / realm / id).DELETE
      Http(unregistration > as.String) must (runUnder(restTimeout) and eventuallySucceed)
      info(s"Deletion for user $handle started")
      eventually {
        val statusCode_> = Http(profileResource).map(_.getStatusCode)
        statusCode_> must (runUnder(restTimeout) and eventually(be(401)))
        info(s"User $handle removed")
      }
      Thread.sleep((2 minutes).toMillis) // FIXME: Wait until user is removed from Infinity, etc.
    }
  }
}
