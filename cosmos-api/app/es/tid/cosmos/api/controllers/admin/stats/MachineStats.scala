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
