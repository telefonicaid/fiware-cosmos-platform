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

import java.io.ByteArrayOutputStream

import com.jcraft.jsch.{KeyPair, JSch}

/**
 * Utility for generating SSH keys.
 */
object SshKeyGenerator {
  private val charset = "US-ASCII"

  private lazy val generator = new JSch()

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
    val pair = KeyPair.genKeyPair(generator, KeyPair.RSA)
    val privateKeyStream = new ByteArrayOutputStream()
    val publicKeyStream = new ByteArrayOutputStream()
    pair.writePrivateKey(privateKeyStream)
    pair.writePublicKey(publicKeyStream, userName)
    pair.dispose()
    SshKeys(
      privateKeyStream.toString(charset),
      publicKeyStream.toString(charset),
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
