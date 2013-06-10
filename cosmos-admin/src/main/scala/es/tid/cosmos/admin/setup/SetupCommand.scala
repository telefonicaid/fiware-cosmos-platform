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

package es.tid.cosmos.admin.setup

import scala.concurrent.Await
import scala.concurrent.duration._
import scala.concurrent.ExecutionContext.Implicits._

import es.tid.cosmos.servicemanager.ServiceManager

class SetupCommand(serviceManager: ServiceManager) {

  private def deployPersistentHdfs() = {
    val hdfsDeploy_> = serviceManager.deployPersistentHdfsCluster()
    hdfsDeploy_>.onSuccess({
      case _ => println("Persistent HDFS successfully deployed!")
    })
    hdfsDeploy_>.onFailure({
      case throwable => {
        println("ERROR: Persistent HDFS deployment failure")
        println(throwable.getMessage)
        println()
        println(throwable.getStackTrace)
      }})
    Await.ready(hdfsDeploy_>, 15 minutes).value.get.isSuccess
  }

  private def setupPersistentHdfs() = {
    println("Checking if Persistent HDFS cluster exists...")
    serviceManager.describeCluster(serviceManager.persistentHdfsId) match {
      case None => {
        println("Persistent HDFS not found. Deploying...")
        deployPersistentHdfs()
      }
      case _ => {
        println("Persistent HDFS found. Not changing anything...")
        true
      }
    }
  }

  def run() = {
    setupPersistentHdfs()
  }
}
