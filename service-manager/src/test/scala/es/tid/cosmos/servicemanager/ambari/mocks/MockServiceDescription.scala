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

import es.tid.cosmos.servicemanager.ambari.services.AmbariServiceDescription
import es.tid.cosmos.servicemanager.ambari.configuration.{ConfigurationBundle, ConfigurationKeys}
import es.tid.cosmos.servicemanager.ambari.rest.{Service, Host, Cluster}
import scala.concurrent.Future
import org.mockito.BDDMockito._
import org.mockito.Matchers._
import scala.concurrent.Future._
import es.tid.cosmos.servicemanager.ComponentDescription
import org.scalatest.mock.MockitoSugar

case class MockServiceDescription(
    name: String,
    components: Seq[ComponentDescription],
    configuration: ConfigurationBundle) extends AmbariServiceDescription with MockitoSugar {

  val serviceMock = mock[Service]
  given(serviceMock.addComponent(any())).willReturn(successful("componentName"))
  given(serviceMock.install()).willReturn(successful(serviceMock))
  given(serviceMock.start()).willReturn(successful(serviceMock))
  given(serviceMock.stop()).willReturn(successful(serviceMock))

  override def contributions(
    properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle = configuration

  override def createService(cluster: Cluster, master: Host, slaves: Seq[Host]): Future[Service] =
    Future.successful(serviceMock)
}
