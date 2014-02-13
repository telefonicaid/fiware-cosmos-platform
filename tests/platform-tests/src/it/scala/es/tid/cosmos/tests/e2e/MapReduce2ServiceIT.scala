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

import es.tid.cosmos.servicemanager.ambari.services.MapReduce2

class MapReduce2ServiceIT extends E2ETestBase with ServiceBehaviors {

  scenariosFor(
    installingServiceAndRunningAnExample(MapReduce2) { cluster =>
      cluster.scp(resource("/mapreduce2-test.sh"))
      cluster.sshCommand("bash ./mapreduce2-test.sh")
    }
  )
}
