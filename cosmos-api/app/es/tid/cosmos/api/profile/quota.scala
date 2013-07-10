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

/**
 * Defines the resource quota, which can be empty, unlimited or finite.
 */
sealed abstract class Quota {
  def withinQuota(request: Int): Boolean
}

case object EmptyQuota extends Quota {
  def withinQuota(request: Int): Boolean = false
}
case object UnlimitedQuota extends Quota {
  def withinQuota(request: Int): Boolean = true
}
case class FiniteQuota(limit: Int) extends Quota {
  require(limit > 0, s"Invalid quota: $limit")
  def withinQuota(request: Int): Boolean = request <= limit
}

object Quota {

  /**
   * Determines what type of quota is applicable for a given input value.
   *
   * @param limit  Can be empty, or if not, the numerical limit of the quota.
   * @return       The type of quota determined by the input limit.
   */
  def apply(limit: Option[Int]): Quota = limit match {
    case None => UnlimitedQuota
    case Some(value) => if (value == 0) EmptyQuota else FiniteQuota(value)
  }
}
