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

package es.tid.cosmos.servicemanager.configuration

import es.tid.cosmos.servicemanager.services.Service

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
 * @param service the service for which configuration ir provided
 */
case class ServiceConfiguration(
    final val configType: String,
    properties: Map[String, Any],
    service: Service) extends Configuration

