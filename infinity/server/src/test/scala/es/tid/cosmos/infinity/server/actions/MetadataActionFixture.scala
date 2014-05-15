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

package es.tid.cosmos.infinity.server.actions

import com.typesafe.config.ConfigFactory
import org.mockito.Mockito._
import org.scalatest.mock.MockitoSugar

import es.tid.cosmos.infinity.common.fs.{FileMetadata, Path}
import es.tid.cosmos.infinity.common.permissions.UserProfile
import es.tid.cosmos.infinity.server.config.MetadataServerConfig
import es.tid.cosmos.infinity.server.hadoop.MockNameNode
import es.tid.cosmos.infinity.server.urls.UrlMapper

trait MetadataActionFixture extends MockitoSugar {
  val on = Path.absolute("/to/file")
  val nameNode = spy(new MockNameNode)
  val urlMapper = mock[UrlMapper]
  val user = UserProfile("gandalf", Seq("istari"))
  val context = Action.Context(user, urlMapper)
  val config = new MetadataServerConfig(ConfigFactory.load())
  val metadata = mock[FileMetadata]
}
