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

package es.tid.cosmos.platform.ial.serverpool

import java.util.concurrent.TimeUnit
import scala.concurrent.{Await, Future}
import scala.concurrent.ExecutionContext.Implicits.global
import scala.concurrent.duration.{FiniteDuration, Duration}
import scala.util.Failure

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

import es.tid.cosmos.platform.ial.{MachineProfile, ResourceExhaustedException}

/**
 * @author apv
 */
class ServerPoolInfrastructureProviderIT extends FlatSpec with ServerPoolSqlTest with MustMatchers {

  val provider = new ServerPoolInfrastructureProvider(dao)
  val timeout: FiniteDuration = Duration(2, TimeUnit.SECONDS)

  "Server pool infrastructure provider" must "create machines when available" in {
    val machines = Await.result(provider.createMachines("cosmos", MachineProfile.M, 2), timeout)
    machines must (containMachine("cosmos0") and containMachine("cosmos1"))
  }

  it must "throw when creating machines when unavailable" in {
    val result = provider.createMachines("pre", MachineProfile.XL, 1500)
    Await.ready(result, timeout)
    result.value match {
      case Some(Failure(ResourceExhaustedException(_, requested, available))) => {
        requested must be (1500)
        available must be (1)
      }
    }
  }
}
