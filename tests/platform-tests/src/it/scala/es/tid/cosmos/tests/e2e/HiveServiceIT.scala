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

package es.tid.cosmos.tests.e2e

class HiveServiceIT extends E2ETestBase with ServiceBehaviors {

  override protected val service = "HIVE"

  scenariosFor(
    installingServiceAndRunningAnExample { cluster =>
      cluster.scp(resource("/hive-test.sh"))
      cluster.sshCommand("bash ./hive-test.sh")
    }
  )
}
