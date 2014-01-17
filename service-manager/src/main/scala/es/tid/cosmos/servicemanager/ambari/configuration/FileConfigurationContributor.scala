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

import com.typesafe.config._

import es.tid.cosmos.servicemanager.ambari.configuration.FactoryTypes._
import es.tid.cosmos.servicemanager.ambari.configuration.FactoryTypes.Implicits._

/**
 * Trait for mixing-in configuration contributions from a file.
 * The loader will load the configuration found in the classpath `[configName].conf` file
 * as specified by `configName`.
 *
 * The Loader makes use of TypeSafe's Config class, so any syntax supported by that class is
 * supported by the FileConfigurationContributor.
 *
 * @see [[https://github.com/typesafehub/config About TypeSafe's config]]
 * @see [[https://github.com/typesafehub/config/blob/master/HOCON.md About HOCON format]]
 */
trait FileConfigurationContributor extends ConfigurationContributor {
  protected val configName: String

  private class ClusterConfigIncluder(
      properties: Map[ConfigurationKeys.Value, String],
      fallback: Option[ConfigIncluder] = None)
    extends ConfigIncluder {

    def withFallback(fallback: ConfigIncluder): ConfigIncluder =
      new ClusterConfigIncluder(properties, Option(fallback))

    def include(context: ConfigIncludeContext, what: String): ConfigObject = what match {
      case "cluster-properties" => ConfigValueFactory.fromMap(
        properties.map(tuple => (tuple._1.toString, tuple._2)))
      case _ => fallback
        .map(_.include(context, what))
        .getOrElse(ConfigValueFactory.fromMap(Map[String,String]()))
    }
  }

  protected def resolveConfig(properties: Map[ConfigurationKeys.Value, String]) =
    ConfigFactory.load(
      this.getClass.getClassLoader,
      configName,
      ConfigParseOptions.defaults().setIncluder(new ClusterConfigIncluder(properties)),
      ConfigResolveOptions.defaults())

  /**
   * Builder of service description configurations.
   * For the service `configName` it loads its configuration from a file and it looks for 3 types of
   * configuration: Global, Core and Service. They are all optional.
   *
   * In order for the configuration file to be able to use dynamic properties, the configuration
   * file must include a `include "cluster-properties` statement.
   *
   * configuration example:
   * {{{
   *
   * include "cluster-properties"
   *
   *"global" {
   *  "tag"="aTag",
   *  "properties" {
   *    "global.example.string"="global-"${MasterNode}
   *   }
   *}
   *"core" {
   *  "tag"="aTag",
   *  "properties" {
   *    "core.example"="core-"${MasterNode}
   *   }
   *}
   *"a-service" {
   *  "configType"="test-service-site"
   *  "tag"="aTag",
   *  "properties" {
   *    "service.example"="service-"${MasterNode}
   *   }
   *}
   * }}}
   *
   * @see [[ConfigurationKeys]]
   */
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

  private def service(config: Config) =
    if (config.hasPath(configName)) {
      val maybeSingleConfig = properties(configName, config).map(
        props => List(ServiceConfiguration(config.getString(s"$configName.configType"), props)))

      def multipleConfig = (for ((configName, props) <- multipleProperties(configName, config))
        yield ServiceConfiguration(configName, props)).toList

      maybeSingleConfig.getOrElse(multipleConfig)
    } else Nil

  private def properties(name: String, config: Config): Option[Map[String, AnyRef]] = {
    val key = s"$name.properties"
    if (config.hasPath(key))
      Some(config.getObject(key).unwrapped().toMap)
    else
      None
  }

  private def multipleProperties(name: String, config: Config): Map[String, Map[String, AnyRef]] =
    (for (innerConfig <- config.getConfigList(name))
      yield innerConfig.getString("configType") -> innerConfig.getObject("properties")
        .unwrapped().toMap
    ).toMap
}
