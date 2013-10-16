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

package es.tid.cosmos.platform.common.scalatest

import org.scalatest.Tag

/**
 * Package containing test tag definitions used for test filtering.
 */
package object tags {

  /**
   * Tag for tests with external dependencies requiring a special running environment
   * e.g. Database instances, system libraries etc.
   */
  object HasExternalDependencies extends Tag("HasExternalDependencies")

  /**
   * Tag for end to end tests which are meant to be run against a functional instance of Cosmos
   */
  object EndToEndTest extends Tag("EndToEndTest")
}
