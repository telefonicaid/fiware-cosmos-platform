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

package es.tid.cosmos.infinity.server.finatra

import com.twitter.finagle.http.Response
import org.jboss.netty.handler.codec.http.HttpResponseStatus

import es.tid.cosmos.infinity.server.actions.Action

/** An object able to render action results into HTTP responses. */
object ActionResultHttpRenderer {

  def apply(result: Action.Result): Response = Response(HttpResponseStatus.OK)
}
