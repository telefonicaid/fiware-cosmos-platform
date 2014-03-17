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

package es.tid.cosmos.admin

import _root_.play.api.Play

import es.tid.cosmos.admin.cli.AdminArguments
import es.tid.cosmos.admin.command.CommandRunnerComponent
import es.tid.cosmos.admin.play.DataAccessApplicationComponent
import es.tid.cosmos.api.profile.dao.sql.PlayDbDataStoreComponent
import es.tid.cosmos.common.ApplicationConfigComponent
import es.tid.cosmos.servicemanager.production.ProductionServiceManagerComponent

object Main extends CommandRunnerComponent
  with DataAccessApplicationComponent
  with ProductionServiceManagerComponent
  with PlayDbDataStoreComponent
  with ApplicationConfigComponent {

  def main(args: Array[String]) {
    Play.start(playApplication)
    val status = try {
      commandRunner(new AdminArguments(args)).run()
    } finally {
      Play.stop()
    }
    System.exit(status)
  }
}
