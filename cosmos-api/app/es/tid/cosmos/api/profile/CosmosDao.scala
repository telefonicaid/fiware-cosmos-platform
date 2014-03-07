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

package es.tid.cosmos.api.profile

/** This trait creates an abstraction on how to use the underlying data, such as SQL databases,
  * in-memory lists, mocks, etc.
  */
trait CosmosDao {

  /** Specifies the type of connection to use to the data. */
  type Conn

  def store: DataStore[Conn]
  def profile: ProfileDao[Conn]
  def capability: CapabilityDao[Conn]
  def group: GroupDao[Conn]
  def cluster: ClusterDao[Conn]
}
