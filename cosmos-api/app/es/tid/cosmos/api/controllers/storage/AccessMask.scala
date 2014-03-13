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

import play.api.libs.json.{JsString, JsValue, Writes}

/** Unix-style permission mask */
case class AccessMask(mask: Int) {
  require(mask >= 0 && mask <= AccessMask.MaxMask, "Out of range mask: %o".format(mask))

  override def toString = "%03o".format(mask)
}

object AccessMask {
  val MaxMask = octal("777")

  def apply(mask: String): AccessMask = AccessMask(octal(mask))

  private def octal(number: String) = Integer.parseInt(number, 8)

  implicit object AccessMaskWrites extends Writes[AccessMask] {
    override def writes(accessMask: AccessMask): JsValue = JsString(accessMask.toString)
  }
}
