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

package es.tid.cosmos.admin

import es.tid.cosmos.api.profile.{UnlimitedQuota, CosmosProfileDao, Quota}

private[admin] class Profile(dao: CosmosProfileDao) {

  def setMachineQuota(cosmosId: Long, limit: Int): Boolean = {
    dao.withConnection { implicit c =>
      dao.setMachineQuota(cosmosId, Quota(Some(limit)))
    }
  }

  def unsetMachineQuota(cosmosId: Long): Boolean = {
    dao.withConnection { implicit c =>
      dao.setMachineQuota(cosmosId, UnlimitedQuota)
    }
  }
}
