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

package es.tid.cosmos.infinity.server.groups

import scala.collection.JavaConverters._

import org.apache.hadoop.conf.Configuration
import org.mockito.BDDMockito.given
import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers
import org.scalatest.mock.MockitoSugar

class GroupMappingProviderTest extends FlatSpec with MustMatchers with MockitoSugar {

  "A group mapping provider" must "throw when used before initialization" in new Fixture {
    evaluating { provider.getConf } must produce [IllegalStateException]
    evaluating { provider.getGroups("user") } must produce [IllegalStateException]
  }

  it must "build a group mapping with the provided hadoop config" in new Fixture {
    provider.setConf(config)
    initialized must be (true)
  }

  it must "delegate on the group mapping" in new Fixture {
    private val groups = Seq("cosmos", "other")
    given(groupMapping.groupsFor("user")).willReturn(groups)
    provider.setConf(config)
    provider.getGroups("user") must be (groups.asJava)
  }

  trait Fixture {
    val groupMapping = mock[GroupMapping]
    val config = new Configuration(false)
    var initialized = false
    val provider = new GroupMappingProvider {
      override def buildGroupMapping(passedConfig: Configuration) = {
        passedConfig must be theSameInstanceAs config
        initialized = true
        groupMapping
      }
    }
  }
}
