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

package es.tid.cosmos.api.auth.oauth2

import scala.language.implicitConversions

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
