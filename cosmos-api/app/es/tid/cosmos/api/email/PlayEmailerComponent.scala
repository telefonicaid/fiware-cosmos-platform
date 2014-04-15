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

package es.tid.cosmos.api.email

import es.tid.cosmos.common.ConfigComponent

/** Component for the PlayEmailer */
trait PlayEmailerComponent extends EmailerComponent{
  this: ConfigComponent =>

  override lazy val emailer = new PlayEmailer(config.getString("email.fromAddress"))
}
