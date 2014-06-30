/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
