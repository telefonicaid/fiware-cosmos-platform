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

package es.tid.cosmos.servicemanager.ambari.mocks

import scala.concurrent.Future._

import org.mockito.BDDMockito._
import org.mockito.Matchers._
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.{Service, NoParametrization, ComponentDescription}
import es.tid.cosmos.servicemanager.ambari.configuration.{ConfigurationBundle, ConfigurationKeys}
import es.tid.cosmos.servicemanager.ambari.services.{AmbariServiceDetails, AmbariService}
import es.tid.cosmos.servicemanager.ambari.rest.ServiceClient

case class MockService(
    name: String,
    components: Seq[ComponentDescription],
    configuration: ConfigurationBundle)
  extends AmbariService with NoParametrization with MockitoSugar {

  val serviceMock = mock[ServiceClient]
  given(serviceMock.addComponent(any())).willReturn(successful("componentName"))
  given(serviceMock.install()).willReturn(successful(serviceMock))
  given(serviceMock.start()).willReturn(successful(serviceMock))
  given(serviceMock.stop()).willReturn(successful(serviceMock))

  override def contributions(
    properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle = configuration

  override def ambariService = new AmbariServiceDetails {
    override val components = MockService.this.components
    override val service = MockService.this
  }
}
