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

package es.tid.cosmos.api.controllers.admin

import scala.util.Try

import com.typesafe.config.Config

import es.tid.cosmos.platform.common.ConfigComponent

/**
 * Keeps maintenance status information in memory taking the initial value from
 * a configuration setting.
 */
private[admin] class InMemoryMaintenanceStatus(config: Config) extends MaintenanceStatus {

  private var isUnderMaintenance =
    Try(config.getBoolean("application.underMaintenance")).getOrElse(false)

  def underMaintenance: Boolean = isUnderMaintenance

  def enterMaintenance() {
    isUnderMaintenance = true
  }

  def leaveMaintenance() {
    isUnderMaintenance = false
  }
}

trait InMemoryMaintenanceStatusComponent extends MaintenanceStatusComponent {
  this: ConfigComponent =>

  val maintenanceStatus: MaintenanceStatus = new InMemoryMaintenanceStatus(config)
}
