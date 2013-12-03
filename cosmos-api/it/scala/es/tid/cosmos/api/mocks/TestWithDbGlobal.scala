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

package es.tid.cosmos.api.mocks

import es.tid.cosmos.api.AbstractGlobal
import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.controllers.admin.InMemoryMaintenanceStatusComponent
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManagerComponent
import es.tid.cosmos.api.profile.PlayDbCosmosProfileDaoComponent
import es.tid.cosmos.platform.common.ApplicationConfigComponent

/**
 * Custom global Play! settings to configure mocked services but use a real database.
 */
class TestWithDbGlobal extends AbstractGlobal(new Application
  with MockMultiAuthProviderComponent
  with PlayDbCosmosProfileDaoComponent
  with MockedServiceManagerComponent
  with MockInfrastructureProvider.Component
  with ApplicationConfigComponent
  with InMemoryMaintenanceStatusComponent)
