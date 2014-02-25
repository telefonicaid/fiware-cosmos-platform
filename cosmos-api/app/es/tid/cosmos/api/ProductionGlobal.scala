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

package es.tid.cosmos.api

import es.tid.cosmos.api.auth.multiauth.ConfigBasedMultiAuthProviderComponent
import es.tid.cosmos.api.auth.request.ChainedAuthenticationComponent
import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.controllers.admin.InMemoryMaintenanceStatusComponent
import es.tid.cosmos.api.profile.PlayDbCosmosProfileDaoComponent
import es.tid.cosmos.api.task.inmemory.InMemoryTaskDaoComponent
import es.tid.cosmos.common.ApplicationConfigComponent
import es.tid.cosmos.servicemanager.production.ProductionServiceManagerComponent

/** Global application settings tied to real services. */
object ProductionGlobal extends AbstractGlobal(new Application
  with ConfigBasedMultiAuthProviderComponent
  with PlayDbCosmosProfileDaoComponent
  with ProductionServiceManagerComponent
  with ApplicationConfigComponent
  with InMemoryMaintenanceStatusComponent
  with ChainedAuthenticationComponent
  with InMemoryTaskDaoComponent) {
}
