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

package es.tid.cosmos.tests.e2e

import org.scalatest.time.{Seconds, Minutes, Span}
import org.scalatest.concurrent.{IntegrationPatience, Eventually}

trait Patience extends Eventually with IntegrationPatience {
  implicit final override val patienceConfig = PatienceConfig(
    timeout = scaled(Span(30, Minutes)),
    interval = scaled(Span(10, Seconds)))
}
