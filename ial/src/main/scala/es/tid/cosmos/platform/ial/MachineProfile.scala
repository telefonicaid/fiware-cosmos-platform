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

package es.tid.cosmos.platform.ial

/**
 * An enumeration of the available machine profiles.
 *
 * Gx refers to the hardware's revision / generation.
 * G1's codename is "Little Joe"
 */
object MachineProfile extends Enumeration {

  type MachineProfile = Value

  /* Uses indices for backwards compability while serializing. */
  val G1Compute = Value(1, "Compute machine version 1")
  val HdfsSlave = Value(2, "Persistent HDFS DataNode")
  val HdfsMaster = Value(3, "Persistent HDFS Namenode")
}
