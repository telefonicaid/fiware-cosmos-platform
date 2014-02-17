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

package es.tid.cosmos.common.scalatest.resources

/** Mixin utility to gain access to test-resource directory paths. */
trait TestResourcePaths {
  /** Exposes the src/[test, it]/resources directory path */
  protected lazy val resourcesConfigDirectory = this.getClass.getClassLoader.getResource("").getPath

  /** Exposes the src/[test, it]/resources/a/package/path directory as the resource path based on the
    * test's package.
    */
  protected lazy val packageResourcesConfigDirectory = this.getClass.getResource("").getPath
}
