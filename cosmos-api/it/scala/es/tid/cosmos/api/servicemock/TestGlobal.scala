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

package es.tid.cosmos.api.servicemock

import es.tid.cosmos.api.AbstractGlobal
import es.tid.cosmos.api.controllers.Application

/**
 * Custom global Play! settings to configure mocked services.
 */
object TestGlobal extends AbstractGlobal(new Application with MockedServiceManagerComponent)