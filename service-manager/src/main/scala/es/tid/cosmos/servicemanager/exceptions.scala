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

package es.tid.cosmos.servicemanager

import com.ning.http.client.Request

class ServiceException(message: String, cause: Throwable = null)
  extends RuntimeException(message, cause)

case class RequestException(request: Request, message: String, cause: Throwable = null)
  extends ServiceException(s"""$message
                               |Request: $request""".stripMargin, cause)

case class DeploymentException(href: String, cause: Throwable = null)
  extends ServiceException(
    "The cluster did not finish installing correctly. Please contact cosmos@tid.es and tell them" +
      s" that deployment failed at $href",
    cause)

case class ServiceError(message: String, cause: Throwable = null) extends Error(message, cause)