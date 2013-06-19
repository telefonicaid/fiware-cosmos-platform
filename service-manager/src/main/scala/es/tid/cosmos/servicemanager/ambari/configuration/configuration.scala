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

/**
 * Abstract representation of a cluster configuration.
 */
trait Configuration extends ConfigurationHeader {
  def properties: Map[String, Any]
}

/**
 * Types for configuration factories.
 */
object FactoryTypes {
  type Factory[T <: Configuration] = Map[String, Any] => T
  object Implicits {
    implicit val globalFactory: Factory[GlobalConfiguration] = GlobalConfiguration
    implicit val coreFactory: Factory[CoreConfiguration] = CoreConfiguration
  }
}

/**
 * Representation of a cluster's global configuration. Can be partial.
 *
 * @param properties the global configuration properties
 */
case class GlobalConfiguration(properties: Map[String, Any]) extends Configuration {
  final val configType: String = "global"
}

/**
 * Representation of a cluster's core configuration. Can be partial.
 *
 * @param properties the core configuration properties
 */
case class CoreConfiguration(properties: Map[String, Any]) extends Configuration {
  final val configType: String = "core-site"
}
/**
 * Representation of a service <b>complete</b> configuration.
 *
 * @param configType the type for this service configuration
 * @param properties the service configuration properties
 */
case class ServiceConfiguration(
    final val configType: String, properties: Map[String, Any]) extends Configuration

/**
 * Container of different types of cluster configuration.
 *
 * @param global the global configuration. Optional and can be partial
 * @param core the core configuration. Optional and can be partial
 * @param services configuration for services. Should be complete configuration for each service
 */
case class ConfigurationBundle(
    global: Option[GlobalConfiguration],
    core: Option[CoreConfiguration],
    services: List[ServiceConfiguration]) {
  /**
   * Get the bundle's concrete configurations, i.e. excluding the optional ones without value.
   *
   * @return the bundle's concrete configurations
   */
  def configurations: List[Configuration] =  List(global, core).flatten ++ services
}

object ConfigurationBundle {
  def apply(global: GlobalConfiguration, services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(Some(global), None, services.toList)

  def apply(core: CoreConfiguration, services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(None, Some(core), services.toList)

  def apply(
    global: GlobalConfiguration,
    core: CoreConfiguration,
    services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(Some(global), Some(core), services.toList)

  def apply(services: ServiceConfiguration*): ConfigurationBundle =
    ConfigurationBundle(None, None, services.toList)
}
