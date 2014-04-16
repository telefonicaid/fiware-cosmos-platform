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
