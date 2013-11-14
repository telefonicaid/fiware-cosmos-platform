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

package es.tid.cosmos.servicemanager.ambari

import java.net.URI
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.Future

import es.tid.cosmos.platform.ial.MachineState
import es.tid.cosmos.servicemanager.ambari.services.Hdfs
import es.tid.cosmos.servicemanager.clusters.HostDetails

/**
 * Functions for handling machines.
 */
package object machines {
  def masterAndSlaves(machines: Seq[MachineState]) = machines match {
    case Seq(master) => (master, machines)
    case Seq(master, slaves @ _*) => (master, slaves)
    case _ => throw new IllegalArgumentException("Need at least one machine")
  }

  def masterAndSlaves_>(machines_> : Future[Seq[MachineState]]) =
    for (machines <- machines_>) yield masterAndSlaves(machines)

  /**
   * Map operation on the master node of the given machines.
   *
   * @param machines_> the machine futures
   * @param f the function to be mapped to the master node
   * @tparam T the result type of the mapping function
   * @return the result obtained by mapping the machines' master to to the given function
   */
  def mapMaster[T](machines_> : Future[Seq[MachineState]], f: MachineState => T): Future[T] =
    for ((master, _) <- masterAndSlaves_>(machines_>)) yield f(master)

  /**
   * Map operation on the slave nodes of the given machines.
   *
   * @param machines_> the machine futures
   * @param f the function to be mapped to the slave nodes
   * @tparam T the result type of the mapping function
   * @return the result obtained by mapping the machines' slaves to to the given function
   */
  def mapSlaves[T](machines_> : Future[Seq[MachineState]], f: MachineState => T): Future[Seq[T]] =
    for ((_, slaves) <- masterAndSlaves_>(machines_>)) yield slaves.map(f)

  /**
   * Create the namenode URI for the given host.
   *
   * @param host the host for whom to create the URI
   * @return the namenode URI e.g. `hdfs://localhost:50070`
   */
  def toNameNodeUri(host: MachineState) =
    new URI(s"hdfs://${host.hostname}:${Hdfs.nameNodeHttpPort}")

  def toHostInfo(host: MachineState) = HostDetails(host.hostname, host.ipAddress)
}
