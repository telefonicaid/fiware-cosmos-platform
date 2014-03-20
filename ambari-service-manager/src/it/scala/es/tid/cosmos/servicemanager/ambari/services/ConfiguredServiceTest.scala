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

package es.tid.cosmos.servicemanager.ambari.services

import java.io.File
import java.nio.file.{StandardCopyOption, Files}

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.common.scalatest.resources.TestResourcePaths
import es.tid.cosmos.servicemanager.ambari.configuration.ConfigurationContributor
import es.tid.cosmos.servicemanager.configuration.ConfigurationKeys

trait ConfiguredServiceTest extends FlatSpec with MustMatchers with TestResourcePaths {

  def dynamicProperties: Map[ConfigurationKeys.Value, String]
  def configurator: ConfigurationContributor

  def contributions = configurator.contributions(dynamicProperties)

  ConfiguredServiceTest.synchronized {
    new File(resourcesConfigDirectory)
      .listFiles()
      .filter(_.getName.endsWith(".erb"))
      .foreach(file => {
        val path = file.getAbsolutePath
        Files.move(
          file.toPath,
          new File(path.take(path.lastIndexOf('.'))).toPath,
          StandardCopyOption.REPLACE_EXISTING)
      })
  }
}

object ConfiguredServiceTest
