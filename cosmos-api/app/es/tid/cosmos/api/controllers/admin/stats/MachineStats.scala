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

package es.tid.cosmos.api.controllers.admin.stats

import play.api.libs.json.{JsObject, Json, JsValue, Writes}

import es.tid.cosmos.platform.ial.MachineProfile
import es.tid.cosmos.platform.ial.MachineProfile.MachineProfile

/** Existing and available machines by profile */
case class MachineStats(profileStats: Map[MachineProfile, ResourceUse]) {
  MachineProfile.values.foreach { profile =>
    require(profileStats.contains(profile), s"Missing stats for profile '$profile'")
  }
}

object MachineStats {

  def apply(profileStats: (MachineProfile, ResourceUse)*): MachineStats =
    MachineStats(profileStats.toMap)

  implicit object MachineStatsWrites extends Writes[MachineStats] {
    override def writes(stats: MachineStats): JsValue = new JsObject(
      for {
        (profile, profileStats) <- stats.profileStats.toSeq
      } yield profile.toString -> Json.toJson(profileStats)
    )
  }
}
