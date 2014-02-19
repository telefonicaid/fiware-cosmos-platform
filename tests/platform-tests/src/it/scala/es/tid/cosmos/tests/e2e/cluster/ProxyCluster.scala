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

package es.tid.cosmos.tests.e2e.cluster

import org.scalatest.Informer

import es.tid.cosmos.tests.e2e.User

class ProxyCluster(clusterSize: Int, val owner: User, services: Seq[String])
                  (implicit factory: Cluster.Factory, info: Informer) extends Cluster {

  private lazy val instance = factory(clusterSize, owner, services)

  override def id = instance.id

  override def sshCommand(command: String, executedBy: User) =
    instance.sshCommand(command, executedBy)

  override def scp(localFile: String, remotePath: String, recursive: Boolean, executedBy: User) =
    instance.scp(localFile, remotePath, recursive, executedBy)

  override def terminate(executedBy: User) = instance.terminate(executedBy)

  override def removeUser(clusterUser: String, executedBy: User) =
    instance.removeUser(clusterUser, executedBy)

  override def isListed(executedBy: User) = instance.isListed(executedBy)

  override def describe(executedBy: User) = instance.describe(executedBy)

  override def addUser(clusterUser: String, executedBy: User) =
    instance.addUser(clusterUser, executedBy)
}
