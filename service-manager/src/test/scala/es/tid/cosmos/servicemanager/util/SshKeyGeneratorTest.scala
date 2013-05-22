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

package es.tid.cosmos.servicemanager.util

import org.scalatest.FlatSpec
import org.scalatest.matchers.MustMatchers

class SshKeyGeneratorTest extends FlatSpec with MustMatchers {
  "A generator" must "create a new pair of public private keys" in {
    val keys1a = SshKeyGenerator.newKeys("user1", "host")
    val keys1b = SshKeyGenerator.newKeys("user1", "host")
    val keys2 = SshKeyGenerator.newKeys("user2", "host")
    keys1a must not equal (keys2)
    keys1b must not equal (keys2)
    keys1a must not equal (keys1b)
    keysMustHaveCorrectContent(keys1a, "user1", "host")
    keysMustHaveCorrectContent(keys1b, "user1", "host")
    keysMustHaveCorrectContent(keys2, "user2", "host")
  }

  def keysMustHaveCorrectContent(keys: SshKeys, username: String, host: String) {
    keys.publicKey.stripLineEnd must (startWith ("ssh-rsa") and endWith (username))
    keys.privateKey.stripLineEnd must (
      startWith ("-----BEGIN RSA PRIVATE KEY-----") and endWith ("-----END RSA PRIVATE KEY-----"))
    keys.authorizedHost must equal(host)
    keys.authorizedKey must equal(s"${keys.publicKey.stripLineEnd}@$host\n")
  }
}
