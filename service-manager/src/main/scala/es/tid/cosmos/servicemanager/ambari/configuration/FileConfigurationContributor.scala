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

package es.tid.cosmos.servicemanager.ambari.configuration

import scala.Some
import scala.collection.JavaConversions._
import scala.collection.JavaConverters._

import com.typesafe.config._

import es.tid.cosmos.servicemanager.ambari.configuration.FactoryTypes._
import es.tid.cosmos.servicemanager.ambari.configuration.FactoryTypes.Implicits._
import es.tid.cosmos.servicemanager.ambari.configuration.Implicits._

/**
 * Trait for mixing-in configuration contributions from a file.
 * The loader will load the configuration found in the classpath `[configName].conf` file
 * as specified by `configName`.
*/
trait FileConfigurationContributor extends ConfigurationContributor {
  protected val configName: String

  private class ClusterConfigIncluder(
      properties: Map[ConfigurationKeys.Value, String],
      fallback: ConfigIncluder = null)
    extends ConfigIncluder {

    def withFallback(fallback: ConfigIncluder): ConfigIncluder =
      new ClusterConfigIncluder(properties, fallback)

    def include(context: ConfigIncludeContext, what: String): ConfigObject = what match {
      case "cluster-properties" => ConfigValueFactory.fromMap(properties.toStringMap)
      case _ if fallback != null => fallback.include(context, what)
      case _ => ConfigValueFactory.fromMap(Map[String,String]().asJava)
    }
  }

  protected def resolveConfig(properties: Map[ConfigurationKeys.Value, String]) =
    ConfigFactory.load(
      classOf[FileConfigurationContributor].getClassLoader,
      configName,
      ConfigParseOptions.defaults().setIncluder(new ClusterConfigIncluder(properties)),
      ConfigResolveOptions.defaults())

  override def contributions(
      properties: Map[ConfigurationKeys.Value, String]): ConfigurationBundle = {
    val config = resolveConfig(properties)
    ConfigurationBundle(
      optional[GlobalConfiguration]("global", config),
      optional[CoreConfiguration]("core", config),
      service(config))
  }

  private def optional[T <: Configuration : Factory](name: String, config: Config): Option[T] =
    properties(name, config).map(implicitly[Factory[T]])

  private def service(config: Config) = properties(configName, config)
    .map(props => ServiceConfiguration(config.getString(s"$configName.configType"), props))
    .toList

  private def properties(name: String, config: Config) = {
    val key = s"$name.properties"
    if (config.hasPath(key))
      Some(config.getObject(key).unwrapped().toMap)
    else
      None
  }
}
