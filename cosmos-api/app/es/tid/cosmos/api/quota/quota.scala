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

package es.tid.cosmos.api.quota

/** Defines the resource quota, which can be empty, unlimited or finite. */
sealed trait Quota {

  /** Check if the given quota is contained within this quota.
   *
   * @param other the quota to be checked against this one
   * @return      true iff the quota provided is within this quota
   */
  def withinQuota(other: Quota): Boolean

  /** Check if the requested number of resources is within this quota.
    *
    * Convenience wrapper of ``withinQuota(Quota)``.
    *
    * @param request the requested number of resources
    * @return        true iff the number of resources is within this quota
    */
  def withinQuota(request: Int): Boolean = withinQuota(Quota(request))

  /** Extend the quota by adding another quota to it.
   *
   * @param other the other quota to be added
   * @return      the new quota representing the sum of the two quotas
   */
  def +(other: Quota): Quota

  /** Reduce the quota by subtracting another quota from it.
   *
   * @param other the other quota to be subtracted
   * @return      the new quota representing this quota reduced by the given quota
   */
  def -(other: Quota): Quota

  /** Get a numeric representation of the quota or None.
   *
   * @return the number of resources represented by this quota.
   *         None represents an unlimited quota
   */
  def toOptInt: Option[Int]
}

/** Representation of a quota that can be either empty (`EmptyQuota`)
  * or have a finite limit (`FiniteQuota`).
  */
sealed trait LimitedQuota extends Quota {

  /** Get a numeric representation of the quota.
    *
    * @return the number of resources represented by this quota.
    */
  def toInt: Int

  override def toOptInt: Option[Int] = Some(toInt)
}

/** Representation of an empty quota, i.e. a quota of zero resources. */
case object EmptyQuota extends LimitedQuota {
  override def withinQuota(request: Quota): Boolean = false

  override def +(other: Quota): Quota = other

  override def -(other: Quota): Quota = this

  override def toInt: Int = 0
}

/** Representation of a quota with no resource limit. */
case object UnlimitedQuota extends Quota {
  override def withinQuota(request: Quota): Boolean = true

  override def +(other: Quota): Quota = this

  override def -(other: Quota): Quota = other match {
    case UnlimitedQuota => throw new IllegalArgumentException(
      "Cannot determine result of UnlimitedQuota - UnlimitedQuota")
    case _ => this
  }

  override def toOptInt: Option[Int] = None
}

/** Representation of a quota with a finite resource limit.
 *
 * @param limit the number of resources this quota is limited to
 */
case class FiniteQuota(limit: Int) extends LimitedQuota {
  require(limit > 0, s"Invalid quota: $limit")

  override def withinQuota(request: Quota): Boolean = request match {
    case EmptyQuota => true
    case UnlimitedQuota => false
    case FiniteQuota(otherLimit) => otherLimit <= limit
  }

  override def +(other: Quota): Quota = other match {
    case EmptyQuota => this
    case UnlimitedQuota => UnlimitedQuota
    case FiniteQuota(otherLimit) => FiniteQuota(limit + otherLimit)
  }

  override def -(other: Quota): Quota = other match {
    case EmptyQuota => this
    case UnlimitedQuota => EmptyQuota
    case FiniteQuota(otherLimit) =>
      if (Math.max(0, limit - otherLimit) == 0) EmptyQuota
      else FiniteQuota(limit - otherLimit)
  }

  override def toInt: Int = limit
}

/** Companion object with utility methods for managing quotas. */
object Quota {

  /** Determines what type of quota is applicable for a given input value.
   *
   * @param limit  Can be empty, or if not, the numerical limit of the quota.
   * @return       The type of quota determined by the input limit.
   */
  def apply(limit: Option[Int]): Quota = limit match {
    case None => UnlimitedQuota
    case Some(value) => apply(value)
  }

  /** Determines what type of quota is applicable for a given input value.
   *
   * @param limit  The numerical limit of the quota.
   * @return       The type of quota determined by the input limit.
   */
  def apply(limit: Int): LimitedQuota = if (limit == 0) EmptyQuota else FiniteQuota(limit)

  /** Calculate the maximum of two given quotas.
   *
   * @param left  the 1st quota
   * @param right the 2nd quota
   * @return      the 1st if it is bigger than the 2nd, the 2nd one otherwise
   */
  def max(left: Quota, right: Quota): Quota = (left, right) match {
    case (EmptyQuota, other) => other
    case (other, EmptyQuota) => other
    case (UnlimitedQuota, _) => UnlimitedQuota
    case (_, UnlimitedQuota) => UnlimitedQuota
    case (FiniteQuota(leftLimit), FiniteQuota(rightLimit)) =>
      if (leftLimit > rightLimit) left else right
  }

  /** Calculate the minimum of two given quotas.
   * @param left  the 1st quota
   * @param right the 2nd quota
   * @return      the 1st if it is smaller than the 2nd, the 2nd one otherwise.
   */
  def min(left: Quota, right: Quota): Quota = {
    val unique = Set(left, right)
    if (unique.size == 1) unique.head else unique.filterNot(_ == max(left, right)).head
  }
}
