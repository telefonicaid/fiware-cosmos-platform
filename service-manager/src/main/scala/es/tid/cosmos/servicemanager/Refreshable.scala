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

package es.tid.cosmos.servicemanager

import scala.concurrent.Future

/**
 * Trait adding refreshing capabilities.
 */
trait Refreshable {
  /**
   * Perform a refresh.
   * Note: this operation can have <b>side effects</b>.
   *
   * @return the completion future of the operation
   */
  def refresh(): Future[Unit]
}
