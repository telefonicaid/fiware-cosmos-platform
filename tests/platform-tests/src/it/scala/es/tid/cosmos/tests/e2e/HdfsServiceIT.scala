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

import es.tid.cosmos.servicemanager.ambari.services.Hdfs

class HdfsServiceIT extends E2ETestBase with ServiceBehaviors {

  scenariosFor(
    installingServiceAndRunningAnExample(Hdfs) { cluster =>
      cluster.scp(resource("/hdfs-test.sh"))
      cluster.sshCommand("bash ./hdfs-test.sh")
    }
  )
}
