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

class ClusterUserManagementIT extends E2ETestBase with CommandLineMatchers {

  feature("A user can manage users on its own clusters") {
    withNewUsers(3) { users =>
      val ownerUser = users(0)
      val notOwnerUser = users(1)
      val otherUser = users(2)
      withNewCluster(1, ownerUser) { cluster =>

        scenario("The cluster owner can add another user to its cluster") {
          cluster.ensureState("running")
          cluster.addUser(notOwnerUser.handle) must runSuccessfully
          eventually {
            cluster.users() must contain (notOwnerUser.handle)
          }
        }

        scenario("A not-owner of the cluster cannot add users") {
          cluster.addUser(otherUser.handle, notOwnerUser) must not (runSuccessfully)
        }

        scenario("The cluster owner cannot add himself") {
          cluster.addUser(ownerUser.handle) must not (runSuccessfully)
        }

        scenario("The cluster owner cannot add another user twice") {
          cluster.addUser(notOwnerUser.handle) must not (runSuccessfully)
        }

        scenario("A not-owner of the cluster cannot remove users") {
          cluster.removeUser(notOwnerUser.handle, otherUser) must not (runSuccessfully)
        }

        scenario("The cluster owner can remove a previously added user") {
          cluster.removeUser(notOwnerUser.handle) must runSuccessfully
          eventually {
            cluster.users() must (not contain notOwnerUser.handle)
          }
        }

        scenario("The cluster owner cannot remove himself") {
          cluster.removeUser(ownerUser.handle) must not (runSuccessfully)
        }

        scenario("The cluster owner cannot remove a non-existing user") {
          cluster.removeUser(otherUser.handle) must not (runSuccessfully)
        }
      }
    }
  }
}
