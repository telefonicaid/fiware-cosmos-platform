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

package es.tid.cosmos.servicemanager.services.dependencies

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.servicemanager.services.{Service, Hdfs, Hive}

class ServiceDependenciesTest extends FlatSpec with MustMatchers {

  "Service dependencies" must "be built from a set of required services" in {
    ServiceDependencies.required(Hive, Hdfs) must
      be (ServiceDependencies(Map[Service, DependencyType](Hive -> Required, Hdfs -> Required)))
  }

  it must "throw when required services are repeated" in {
    evaluating {
      ServiceDependencies.required(Hive, Hive)
    } must produce [IllegalArgumentException]
  }

  it must "be build from a set of optional services" in {
    ServiceDependencies.optional(Hive, Hdfs) must
      be (ServiceDependencies(Map[Service, DependencyType](Hive -> Optional, Hdfs -> Optional)))
  }

  it must "throw when adding services already depended upon" in {
    val dependencies = ServiceDependencies.required(Hive, Hdfs)
    evaluating {
      dependencies.optional(Hive)
    } must produce [IllegalArgumentException]
  }
}
