package es.tid.cosmos.api

import es.tid.cosmos.api.controllers.Application
import es.tid.cosmos.api.sm.MockedServiceManagerComponent

/**
 * Application configured with mocked services to be tested in isolation.
 *
 * @author sortega
 */
object TestApplication
  extends Application
  with MockedServiceManagerComponent
