/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

  /* Uses indices for backwards compatibility while serializing. */
  val G1Compute = Value(1, "g1-compute")
  val HdfsSlave = Value(2, "hdfs-slave")
  val HdfsMaster = Value(3, "hdfs-master")
}
