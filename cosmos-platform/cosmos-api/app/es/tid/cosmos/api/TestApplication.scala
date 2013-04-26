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
import es.tid.cosmos.api.servicemanager.MockedServiceManagerComponent

/**
 * Application configured with mocked services to be tested in isolation.
 *
 * @author sortega
 */
object TestApplication
  extends Application
  with MockedServiceManagerComponent
