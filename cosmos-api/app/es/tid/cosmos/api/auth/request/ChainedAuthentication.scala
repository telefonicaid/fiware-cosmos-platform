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

import play.api.mvc.RequestHeader

private[request] class ChainedAuthentication(chain: Seq[RequestAuthentication])
  extends RequestAuthentication {

  require(!chain.isEmpty, "At least one authentication method is required")

  /** Authenticates a request chaining the composed objects.
    *
    * Either the first successful authentication (if any) or the first authentication error is
    * returned.
    */
  override def authenticateRequest(request: RequestHeader): AuthResult = {
    val lazyResults = chain.view.map(auth => auth.authenticateRequest(request))
    val firstSuccess = lazyResults.find(_.isSuccess)
    firstSuccess.getOrElse(lazyResults.head)
  }
}
