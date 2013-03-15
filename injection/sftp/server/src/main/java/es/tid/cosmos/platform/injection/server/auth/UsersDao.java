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

package es.tid.cosmos.platform.injection.server.auth;

import java.security.PublicKey;

/**
 * Data Access Layer for retrieve Passwords and Public Key of Users
 *
 * @author ag453
 */
public interface UsersDao {

    /**
     * Returns the hash of user password
     * on format encoding$seed$passwordHash
     *
     * @param username Username used on login process.
     * @return user's password on encoding$seed$passwordHash format
     */
    String findPasswordByUsername(String username) throws Exception;

    /**
     * Return the Public Keys of user
     *
     * @param username
     * @return Iterable of user's public keys
     */
    Iterable<PublicKey> findPublicKeyByUsername(String username)
            throws Exception;
}
