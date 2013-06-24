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

package es.tid.cosmos.api.oauth2

import com.ning.http.client.{AsyncCompletionHandler, Response, Request, RequestBuilder}
import dispatch.StatusCode

/**
 * Dispatch handler for checking OAuth 2.0 errors.
 */
class OAuthTupleBuilder(builder: RequestBuilder) {
  def OAuthOK[T](f: Response => T): (Request, AsyncCompletionHandler[T]) = {
    (builder.build(), new FunctionHandler(f))
  }

  private class FunctionHandler[T](f: Response => T) extends AsyncCompletionHandler[T] {
    def onCompleted(response: Response): T = (response.getStatusCode / 100) match {
      case 2 => f(response)
      case 4 => {
        val ex = OAuthException.fromForm(response.getResponseBody)
          .getOrElse(StatusCode(response.getStatusCode))
        throw ex
      }
      case _ => throw StatusCode(response.getStatusCode)
    }
  }
}

object OAuthTupleBuilder {
  implicit def implyOAuthTupleBuilder(builder: RequestBuilder): OAuthTupleBuilder =
    new OAuthTupleBuilder(builder)
}
