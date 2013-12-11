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
import es.tid.cosmos.admin.play.DataAccessApplication
import es.tid.cosmos.platform.common.ApplicationConfigComponent
import es.tid.cosmos.servicemanager.ProductionServiceManagerComponent

object ServiceManagerProvider extends ProductionServiceManagerComponent
  with ApplicationConfigComponent

object Main {

  def main(args: Array[String]) {
    val app = new DataAccessApplication(ServiceManagerProvider.config)
    Play.start(app)
    val runner = new CommandRunner(new AdminArguments(args), ServiceManagerProvider.serviceManager)
    val status = runner.run()
    Play.stop()
    System.exit(status)
  }
}
