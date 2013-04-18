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

package es.tid.cosmos.injection.server.auth;

import java.security.PublicKey;

import com.google.common.collect.Iterables;
import org.apache.sshd.server.PublickeyAuthenticator;
import org.apache.sshd.server.session.ServerSession;

import es.tid.cosmos.base.util.Logger;

/**
 * Implements Public Key authentication
 *
 * @author ag453
 */
public class DaoPublicKeyAuthenticator implements PublickeyAuthenticator {

    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(DaoPublicKeyAuthenticator.class);

    private final UsersDao userDao;

    public DaoPublicKeyAuthenticator(UsersDao userDao) {
        this.userDao = userDao;
    }

    /**
     * Authenticate the user on system
     *
     * @param username username used to login
     * @param publicKey RSA or DSA Public Key of user
     * @param unused Session object
     * @return true if password passed as parameter is correct
     */
    @Override
    public boolean authenticate(
            String username, PublicKey publicKey, ServerSession unused) {
        try {
            Iterable<PublicKey> pks =
                    this.userDao.findPublicKeyByUsername(username);
            return Iterables.contains(pks, publicKey);
        } catch (Exception e) {
            LOGGER.error(e.getMessage(), e);
            return false;
        }
    }
}
