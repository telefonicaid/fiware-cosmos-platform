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

package es.tid.cosmos.platform.manager.ial.serverpool

import java.util.concurrent.TimeUnit
import scala.concurrent.Await

import org.scalatest.matchers.MustMatchers
import org.scalatest.FlatSpec

import scala.concurrent.duration.{FiniteDuration, Duration}
import scala.util.Failure

import es.tid.cosmos.platform.manager.ial.{ResourceExhaustedException, MachineProfile}

/**
 * @author apv
 */
class ServerPoolInfrastructureProviderIT extends FlatSpec with MySqlTest with MustMatchers {

  val provider = new ServerPoolInfrastructureProvider(dao)
  val timeout: FiniteDuration = Duration(2, TimeUnit.SECONDS)

  "Server pool infrastructure provider" must "create machines when available" in {
    val result = provider.createMachines("cosmos", MachineProfile.M, 2)
    result must be ('success)
    val machines = result.get.map(Await.result(_, timeout))
    machines must (containMachine("cosmos0") and containMachine("cosmos1"))
  }

  it must "throw when creating machines when unavailable" in {
    val result = provider.createMachines("pre", MachineProfile.XL, 1500)
    result must be ('failure)
    (result match {
      case Failure(ResourceExhaustedException(r, 1500, 1)) => true
      case _ => false
    }) must be (true)
  }
}
