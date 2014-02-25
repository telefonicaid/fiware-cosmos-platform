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
import es.tid.cosmos.api.auth.request.ChainedAuthenticationComponent
import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.mocks.servicemanager.MockedServiceManagerComponent
import es.tid.cosmos.api.controllers.admin.InMemoryMaintenanceStatusComponent
import es.tid.cosmos.api.profile.{CosmosProfileDaoComponent, PlayDbCosmosProfileDaoComponent, MockCosmosProfileDaoComponent}
import es.tid.cosmos.api.task.inmemory.InMemoryTaskDaoComponent
import es.tid.cosmos.common.ApplicationConfigComponent

abstract class TestApplication extends Application
  with MockMultiAuthProviderComponent
  with MockedServiceManagerComponent
  with MockInfrastructureProvider.Component
  with ApplicationConfigComponent
  with InMemoryMaintenanceStatusComponent
  with ChainedAuthenticationComponent
  with InMemoryTaskDaoComponent {

  self: CosmosProfileDaoComponent =>

  val global = new AbstractGlobal(this) {}
}

class MockDaoTestApplication extends TestApplication with MockCosmosProfileDaoComponent

class PlayDaoTestApplication extends TestApplication with PlayDbCosmosProfileDaoComponent
