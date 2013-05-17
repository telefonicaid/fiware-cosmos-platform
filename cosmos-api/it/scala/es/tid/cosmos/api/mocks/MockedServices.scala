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

import play.api.test.FakeApplication
import play.api.test.Helpers.running

/**
 * Replace underlying services with mock implementations.
 */
trait MockedServices {
  def runWithMockedServices[T] = runWithMockedServicesAndConfig[T](Map())_

  def runWithMockedServicesAndConfig[T](configOverrides: Map[String, String] = Map())(block: => T) = {
    val app: FakeApplication = FakeApplication(
      withGlobal = Some(TestGlobal),
      additionalConfiguration =  Map(
        "application.secret" -> "appsecret",
        "db.default.driver" -> "org.h2.Driver",
        "db.default.url" -> "jdbc:h2:mem:test;MODE=MySQL"
      ) ++ configOverrides
    )
    running[T](app)(block)
  }

  def runWithMockedIdentityService[T](block: IdentityService => T) = {
    val identityService = new IdentityService
    identityService.start()
    try {
      runWithMockedServicesAndConfig[T](Map(
        "tuid.auth.url" -> identityService.baseUrl,
        "tuid.api.url" -> identityService.baseUrl,
        "tuid.client.id" -> identityService.clientId,
        "tuid.client.secret" -> identityService.clientSecret
      ))(block(identityService))
    } finally {
      identityService.stop()
    }
  }
}
