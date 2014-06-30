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

import java.io.ByteArrayOutputStream

import com.jcraft.jsch.{KeyPair, JSch}

/**
 * Utility for generating SSH keys.
 */
object SshKeyGenerator {
  private val Charset = "US-ASCII"
  private val KeySizeInBits = 2048
  private lazy val Generator = new JSch()

  /**
   * Create a new set of RSA-based keys. For the given user name it creates
   * a private and public key. It also generates an entry that can be used in an
   * `authorized_keys` file for password-less login from the given host.
   *
   * @param userName the user name
   * @param authorizedHost the host to be used for the authorized_keys entry
   * @return the generated SSH keys
   */
  def newKeys(userName: String, authorizedHost: String): SshKeys = {
    val pair = KeyPair.genKeyPair(Generator, KeyPair.RSA, KeySizeInBits)
    val privateKeyStream = new ByteArrayOutputStream()
    val publicKeyStream = new ByteArrayOutputStream()
    pair.writePrivateKey(privateKeyStream)
    pair.writePublicKey(publicKeyStream, userName)
    pair.dispose()
    SshKeys(
      privateKeyStream.toString(Charset),
      publicKeyStream.toString(Charset),
      authorizedHost
    )
  }
}

/**
 * Case class for holding a user's SSH keys. It also creates an entry for the `authorized_keys` file
 * to allow password-less entry from the given host.
 *
 * @param privateKey the user's private key
 * @param publicKey the user's public key
 * @param authorizedHost the host that should be used for creating an `authorized_keys` entry
 */
case class SshKeys(privateKey: String, publicKey: String, authorizedHost: String) {
  val authorizedKey = s"${publicKey.stripLineEnd}@$authorizedHost\n"
}
