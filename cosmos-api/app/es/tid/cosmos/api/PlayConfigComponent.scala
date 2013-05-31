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

package es.tid.cosmos.api

import com.typesafe.config.Config
import play.api.Play

import es.tid.cosmos.platform.common.ConfigComponent

/**
 * Component that pulls configuration from Play! configuration.
 */
trait PlayConfigComponent extends ConfigComponent {

  def config: Config = Play.current.configuration.underlying
}
