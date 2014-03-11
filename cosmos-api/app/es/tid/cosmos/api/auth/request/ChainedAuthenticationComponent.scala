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

package es.tid.cosmos.api.auth.request

import es.tid.cosmos.api.auth.multiauth.MultiAuthProviderComponent
import es.tid.cosmos.api.profile.dao.CosmosDataStoreComponent

trait ChainedAuthenticationComponent extends RequestAuthenticationComponent {
  this: CosmosDataStoreComponent with MultiAuthProviderComponent =>

  override lazy val apiRequestAuthentication: RequestAuthentication = {
    val baseAuthentications = Seq(
      new ApiCredentialsAuthentication(store),
      new SessionCookieAuthentication(store)
    )
    val optionalAuthentication = multiAuthProvider.tokenAuthenticationProvider.map(authProvider =>
      new TokenAuthentication(authProvider, store)
    )
    new ChainedAuthentication(baseAuthentications ++ optionalAuthentication)
  }
}
