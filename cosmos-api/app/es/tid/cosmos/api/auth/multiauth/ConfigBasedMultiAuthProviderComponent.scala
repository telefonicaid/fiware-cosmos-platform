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

package es.tid.cosmos.api.auth.multiauth

import es.tid.cosmos.common.ConfigComponent

trait ConfigBasedMultiAuthProviderComponent extends MultiAuthProviderComponent {
  this: ConfigComponent =>

  override val multiAuthProvider: MultiAuthProvider = new ConfigBasedMultiAuthProvider(config)
}
