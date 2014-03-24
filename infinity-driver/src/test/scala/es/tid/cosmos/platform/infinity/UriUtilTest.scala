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
