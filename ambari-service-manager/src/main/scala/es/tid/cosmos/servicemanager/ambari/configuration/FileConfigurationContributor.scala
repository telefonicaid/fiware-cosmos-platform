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

import java.io.File
import scala.collection.JavaConversions._

import com.typesafe.config._

import es.tid.cosmos.servicemanager.configuration._
import es.tid.cosmos.servicemanager.configuration.FactoryTypes.Factory
import es.tid.cosmos.servicemanager.configuration.FactoryTypes.Implicits._
import es.tid.cosmos.servicemanager.services.Service

/** Read configuration contributions from file.
  *
  * The loader will load the configuration found in the classpath `[configName].conf` file
  * as specified by `configName`.
  *
  * The Loader makes use of TypeSafe's Config class, so any syntax supported by that class is
  * supported by the FileConfigurationContributor.
  *
  * @see [[https://github.com/typesafehub/config About TypeSafe's config]]
  * @see [[https://github.com/typesafehub/config/blob/master/HOCON.md About HOCON format]]
  */
class FileConfigurationContributor(
    configPath: String,
    configName: String,
    associatedService: Option[Service] = None,
    extraProperties: ConfigProperties = Map.empty)
  extends ConfigurationContributor {

  private class ClusterConfigIncluder(
      properties: ConfigProperties,
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

  protected def resolveConfig(properties: ConfigProperties) =
    ConfigFactory.parseFileAnySyntax(
      new File(s"$configPath/$configName"),
      ConfigParseOptions.defaults().setIncluder(new ClusterConfigIncluder(properties))
    ).resolve()

  /**
   * Builder of service description configurations.
   * For the service `configName` it loads its configuration from a file and it looks for 3 types of
   * configuration: Global, Core and Service. They are all optional.
   *
   * The Service configuration type can be either come in the form of a single configuration or a
   * list of configurations.
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
   *
   *"core" {
   *  "tag"="aTag",
   *  "properties" {
   *    "core.example"="core-"${MasterNode}
   *   }
   *}
   *
   *# Option A: Single configuration for service
   *"a-service" {
   *  "configType"="test-service-site"
   *  "tag"="aTag",
   *  "properties" {
   *    "service.example"="service-"${MasterNode}
   *   }
   *}
   *
   * # Option B: List of configurations for service
   *"a-service" = [
   *  {
   *    "configType"="test-service-site1"
   *    "tag"="aTag",
   *    "properties" {
   *      "service.example1"="service-"${MasterNode}
   *     }
   *  },
   *
   *  {
   *    "configType"="test-service-site2"
   *    "tag"="aTag",
   *    "properties" {
   *      "service.example2"="service-"${MasterNode}
   *     }
   *  }
   *]
   *}}}
   *
   * @param properties the dynamic properties to be injected to the configuration contributions
   * @see [[ConfigurationKeys]]
   */
  override def contributions(properties: ConfigProperties): ConfigurationBundle = {
    val config = resolveConfig(properties ++ extraProperties)
    ConfigurationBundle(
      optional[GlobalConfiguration]("global", config),
      optional[CoreConfiguration]("core", config),
      service(config))
  }

  private def optional[T <: Configuration : Factory](name: String, config: Config): Option[T] =
    properties(name, config).map(implicitly[Factory[T]])

  private def service(config: Config) =
    if (config.hasPath(configName)) {
      require(associatedService.isDefined, "A file configuration contributor cannot " +
        "contribute to a service if no associated service exists")
      val maybeSingleConfig = properties(configName, config).map(
        props => List(ServiceConfiguration(
          config.getString(s"$configName.configType"),props, associatedService.get)))

      def multipleConfig = (for ((configName, props) <- multipleProperties(configName, config))
        yield ServiceConfiguration(configName, props, associatedService.get)).toList

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
