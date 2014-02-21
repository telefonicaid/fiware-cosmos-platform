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

/** Represents length-bound cluster names. */
case class ClusterName(underlying: String) {
  require(underlying.length <= ClusterName.MaxLength,
    s"'$underlying' is longer than ${ClusterName.MaxLength} characters")

  override def toString = underlying
}

object ClusterName {
  val MaxLength = 120
}
