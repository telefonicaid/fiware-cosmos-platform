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

package es.tid.cosmos.platform.infinity

import java.net.{URISyntaxException, URI}

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class UriUtilTest extends FlatSpec with MustMatchers {

  "Scheme replacement" must "keep all other fields" in {
    val ftpUri = URI.create("ftp://user:pass@host:8080/path#tag?foo=bar")
    val httpUri = URI.create("http://user:pass@host:8080/path#tag?foo=bar")
    UriUtil.replaceScheme(ftpUri, "http") must equal (httpUri)
  }

  it must "work when some fields are missing" in {
    val ftpUri = URI.create("ftp:///path")
    val httpUri = URI.create("http:///path")
    UriUtil.replaceScheme(ftpUri, "http") must equal (httpUri)
  }

  "Authority replacement" must "keep all other fields" in {
    val hostUri = URI.create("http://user:pass@host:8080/path#tag?foo=bar")
    val otherHostUri = URI.create("http://otherHost:1234/path#tag?foo=bar")
    UriUtil.replaceAuthority(hostUri, "otherHost:1234") must equal (otherHostUri)
  }

  "Path replacement" must "keep all other fields" in {
    val hostUri = URI.create("http://user:pass@host:8080/old/path#tag?foo=bar")
    val otherPathUri = URI.create("http://user:pass@host:8080/new/path#tag?foo=bar")
    UriUtil.replacePath(hostUri, "/new/path") must equal (otherPathUri)
  }

  it must "fail if a relative path is used" in {
    val hostUri = URI.create("http://user:pass@host:8080/old/path#tag?foo=bar")
    evaluating {
      UriUtil.replacePath(hostUri, "new/path")
    } must produce [URISyntaxException]
  }
}
