/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.api.controllers.infinity

import play.api.libs.json.{JsString, JsValue, Writes}

import es.tid.cosmos.common.Octal

/** Unix-style permission mask */
private[infinity] case class AccessMask(mask: Int) {
  require(mask >= 0 && mask <= AccessMask.MaxMask, "Out of range mask: %o".format(mask))

  override def toString = "%03o".format(mask)
}

private[infinity] object AccessMask {
  val MaxMask = Octal("777")

  def apply(mask: String): AccessMask = AccessMask(Octal(mask))

  implicit object AccessMaskWrites extends Writes[AccessMask] {
    override def writes(accessMask: AccessMask): JsValue = JsString(accessMask.toString)
  }
}
