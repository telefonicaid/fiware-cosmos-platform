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

package es.tid.cosmos.servicemanager.util

import com.jcraft.jsch.{KeyPairRSA, KeyPair, JSch}
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

  it must "create keys of size 2048 bits" in {
    val keys = SshKeyGenerator.newKeys("user1", "host")
    val generator = new JSch()
    val pair = KeyPair.load(generator, keys.privateKey.getBytes, keys.publicKey.getBytes)
    pair.asInstanceOf[KeyPairRSA].getKeySize must equal(2048)
  }

  def keysMustHaveCorrectContent(keys: SshKeys, username: String, host: String) {
    keys.publicKey.stripLineEnd must (startWith ("ssh-rsa") and endWith (username))
    keys.privateKey.stripLineEnd must (
      startWith ("-----BEGIN RSA PRIVATE KEY-----") and endWith ("-----END RSA PRIVATE KEY-----"))
    keys.authorizedHost must equal(host)
    keys.authorizedKey must equal(s"${keys.publicKey.stripLineEnd}@$host\n")
  }
}
