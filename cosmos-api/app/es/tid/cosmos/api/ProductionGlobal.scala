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


import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.oauth2.DefaultMultiOAuthProviderComponent
import es.tid.cosmos.api.profile.PlayDbCosmosProfileDaoComponent
import es.tid.cosmos.platform.common.ApplicationConfigComponent
import es.tid.cosmos.servicemanager.ProductionServiceManagerComponent

/**
 * Global application settings tied to real services.
 */
object ProductionGlobal extends AbstractGlobal(new Application
  with DefaultMultiOAuthProviderComponent
  with PlayDbCosmosProfileDaoComponent
  with ProductionServiceManagerComponent
  with ApplicationConfigComponent) {
}
