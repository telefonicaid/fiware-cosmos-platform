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

package es.tid.cosmos.infinity.client

import java.net.URL

import com.ning.http.client.RequestBuilder
import dispatch.{Future => _, _}

import es.tid.cosmos.infinity.common.credentials.{ClusterCredentials, UserCredentials, Credentials}
import es.tid.cosmos.infinity.common.fs._
import es.tid.cosmos.common.{BearerToken, BasicAuth}

class ResourceBuilder(metadataEndpoint: URL, credentials: Credentials) {

  def metadata(path: Path): RequestBuilder = withAuthHeader(path match {
    case RootPath => metadataRequestBuilder()
    case SubPath(parentPath, name) => metadata(parentPath) / name
  })

  def jsonMetadata(path: Path): RequestBuilder =
    metadata(path).setHeader("Content-Type", "application/json")

  private def metadataRequestBuilder(): RequestBuilder =
    url(metadataEndpoint.toString) / "infinityfs" / "v1" / "metadata"

  def content(location: URL): RequestBuilder = withAuthHeader(url(location.toString))

  private def withAuthHeader(builder: RequestBuilder): RequestBuilder =
    builder.setHeader("Authorization", credentials match {
      case UserCredentials(key, secret) => BasicAuth(key, secret)
      case ClusterCredentials(token) => BearerToken(token)
    })
}
