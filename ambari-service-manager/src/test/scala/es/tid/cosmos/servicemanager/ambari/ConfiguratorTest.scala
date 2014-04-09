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

package es.tid.cosmos.servicemanager.ambari

import org.mockito.BDDMockito.given
import org.mockito.Mockito.{verify, verifyNoMoreInteractions}
import org.mockito.Matchers.{eq => isEq, any, matches}
import org.scalatest.{OneInstancePerTest, FlatSpec}
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.servicemanager.ambari.Configurator.ConfigurationConflict
import es.tid.cosmos.servicemanager.ambari.ConfiguratorTestHelpers.contributionsWithNumber
import es.tid.cosmos.servicemanager.ambari.configuration._
import es.tid.cosmos.servicemanager.ambari.rest.Cluster
import es.tid.cosmos.servicemanager.configuration.{ConfigurationBundle, ServiceConfiguration, CoreConfiguration, GlobalConfiguration}

class ConfiguratorTest extends FlatSpec with OneInstancePerTest with MustMatchers
    with MockitoSugar {

  val cluster = mock[Cluster]
  val noProperties: ConfigProperties = Map.empty

  "A Configurator" must "not apply configuration when there is none available" in {
    Configurator.applyConfiguration(cluster, noProperties, contributors = List())
    verifyNoMoreInteractions(cluster)
  }

  it must "configure using only one contributor" in {
    val contributor = mock[ConfigurationContributor]
    given(contributor.contributions(any())).willReturn(contributionsWithNumber(1))
    Configurator.applyConfiguration(cluster, noProperties, List(contributor))
    verify(cluster).applyConfiguration(isEq(contributionsWithNumber(1).global.get), tagPattern)
    verify(cluster).applyConfiguration(isEq(contributionsWithNumber(1).core.get), tagPattern)
    verify(cluster).applyConfiguration(isEq(contributionsWithNumber(1).services(0)), tagPattern)
  }

  it must "configure using multiple non-overlapping contributors" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    given(contributor1.contributions(any())).willReturn(contributionsWithNumber(1))
    given(contributor2.contributions(any())).willReturn(contributionsWithNumber(2))
    Configurator.applyConfiguration(cluster, noProperties, List(contributor1, contributor2))
    verify(cluster).applyConfiguration(
      isEq(GlobalConfiguration(
        Map("someGlobalContent1" -> "somevalue1", "someGlobalContent2" -> "somevalue2"))),
        tagPattern)
    verify(cluster).applyConfiguration(
      isEq(CoreConfiguration(
        Map("someCoreContent1" -> "somevalue1", "someCoreContent2" -> "somevalue2"))),
      tagPattern)
    verify(cluster).applyConfiguration(
      isEq(ServiceConfiguration("service-site1", Map("someServiceContent1" -> "somevalue1"))), tagPattern)
    verify(cluster).applyConfiguration(
      isEq(ServiceConfiguration("service-site2", Map("someServiceContent2" -> "somevalue2"))), tagPattern)
  }

  it must "fail when more than one service configurations have the same type" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    val configuration1 = contributionsWithNumber(1)
    val configuration2 = contributionsWithNumber(2)
    given(contributor1.contributions(any())).willReturn(configuration1)
    given(contributor2.contributions(any())).willReturn(
      ConfigurationBundle(configuration2.global, configuration2.core, configuration1.services))
    evaluating {
      Configurator.applyConfiguration(cluster, noProperties, List(contributor1, contributor2))
    } must produce [ConfigurationConflict]
  }

  it must "fail when global configuration contributions have property conflicts" in {
    val contributor1 = mock[ConfigurationContributor]
    val contributor2 = mock[ConfigurationContributor]
    val configuration1 = contributionsWithNumber(1)
    val configuration2 = contributionsWithNumber(2)
    given(contributor1.contributions(any())).willReturn(configuration1)
    given(contributor2.contributions(any())).willReturn(
      ConfigurationBundle(configuration1.global, configuration2.core, configuration2.services))
    evaluating {
      Configurator.applyConfiguration(cluster, noProperties, List(contributor1, contributor2))
    } must produce [ConfigurationConflict]
  }

//  it must "apply a configuration with dynamic properties created from hadoop configuration" in {
//    val hadoopConfig = HadoopConfig(mappersPerSlave = 4, reducersPerSlave = 8, zookeeperPort = 1234)
//    val contributor = mock[ConfigurationContributor]
//    val configuration = ConfigurationBundle(GlobalConfiguration(Map.empty))
//    given(contributor.contributions(any())).willReturn(configuration)
//    val master, slave = mock[Host]
//    given(master.name).willReturn("master")
//    given(slave.name).willReturn("slave")
//    Configurator.applyConfiguration(cluster, master, Seq(slave), hadoopConfig, List(contributor))
//    verify(contributor).contributions(Map(
//      HdfsReplicationFactor -> "1",
//      MappersPerSlave -> "4",
//      MasterNode -> "master",
//      MaxReduceTasks -> "14",
//      MaxMapTasks -> "4",
//      ReducersPerSlave -> "8",
//      ZookeeperHosts -> "slave:1234",
//      ZookeeperPort -> "1234"
//    ))
//  }

  def tagPattern = matches("version\\d+")
}
