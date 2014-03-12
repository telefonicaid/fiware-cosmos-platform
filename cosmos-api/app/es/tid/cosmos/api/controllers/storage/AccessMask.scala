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

package es.tid.cosmos.api.controllers.storage

/** Unix-style permission mask */
case class AccessMask(mask: Int) {
  require(mask >= 0 && mask <= AccessMask.MaxMask, "Out of range mask: %o".format(mask))

  override def toString = "%03o".format(mask)
}

object AccessMask {
  val MaxMask = 511
  def apply(mask: String): AccessMask = AccessMask(Integer.parseInt(mask, 8))
}
