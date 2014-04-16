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

package es.tid.cosmos.servicemanager

package object services {

  /** All services registered.
    *
    * Nice-to-have: This could be retrieved by refection once services are organized in only
    * one ambari-independent package.
    */
  lazy val ServiceCatalogue: Set[Service] = Set(
    CosmosUserService,
    HCatalog,
    Hdfs,
    Hive,
    InfinityDriver,
    InfinityServer,
    MapReduce2,
    Oozie,
    Pig,
    Sqoop,
    WebHCat,
    Yarn,
    Zookeeper
  )
}
