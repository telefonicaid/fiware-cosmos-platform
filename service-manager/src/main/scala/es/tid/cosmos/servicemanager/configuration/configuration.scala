/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

