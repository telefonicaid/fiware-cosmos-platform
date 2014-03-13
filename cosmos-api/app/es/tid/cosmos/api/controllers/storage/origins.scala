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

import play.api.libs.json._

/** A given authentication is valid when requests are performed from certain origins. */
sealed trait OriginPolicy
case object AnyHost extends OriginPolicy
case class WhiteList(hosts: Set[String]) extends OriginPolicy

object OriginPolicy {
  implicit object OriginPolicyWrites extends Writes[OriginPolicy] {
    override def writes(policy: OriginPolicy): JsValue = policy match {
      case AnyHost => JsString("anyHost")
      case WhiteList(hosts) => JsArray(hosts.toSeq.sorted.map(JsString.apply))
    }
  }
}
