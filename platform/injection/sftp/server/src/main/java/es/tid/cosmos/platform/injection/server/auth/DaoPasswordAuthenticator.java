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

import java.util.regex.Pattern;

import org.apache.commons.codec.digest.DigestUtils;
import org.apache.sshd.server.PasswordAuthenticator;
import org.apache.sshd.server.session.ServerSession;

import es.tid.cosmos.base.util.Logger;

/**
 * Checks that the user supplied password is found in the HUE frontend database.
 *
 * @author ag453
 */
public class DaoPasswordAuthenticator implements PasswordAuthenticator {

    private static final String DJANGO_SEPARATOR = Pattern.quote("$");
    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(DaoPasswordAuthenticator.class);

    private final UsersDao userDao;

    public DaoPasswordAuthenticator(UsersDao userDao) {
        this.userDao = userDao;
    }

    /**
     * Authenticate the user with user/password method.
     *
     * @param username Username on login process
     * @param password password on plain text.
     * @param unused Session values.
     * @return boolean value that indicates if user and password are allowed
     * on system
     */
    @Override
    public boolean authenticate(
            String username, String password, ServerSession unused) {
        String encodedPassword;
        try {
            encodedPassword = this.findUserPassword(username);
        } catch (Exception e) {
            return false;
        }
        String algorithm = this.getAlgorithm(encodedPassword);
        String hash = this.getHash(encodedPassword);
        String candidateHash;
        if (algorithm.equals("sha1")) {
            candidateHash = this.getCandidateSHA1Hash(encodedPassword,
                    password);
        } else if (algorithm.equals("md5")) {
            candidateHash = this.getCandidateMD5Hash(encodedPassword, password);
        } else {
            LOGGER.warn("Unknown algorithm found in DB: " + algorithm);
            return false;
        }
        return this.isEquals(hash, candidateHash);
    }

    private static boolean isEquals(String hash, String candidateHash) {
        return hash.equals(candidateHash);
    }

    private static String getHash(String encodedPassword) {
        return encodedPassword.split(DJANGO_SEPARATOR)[2];
    }

    private String getCandidateMD5Hash(
            String encodedPassword, String password) {
        return DigestUtils.md5Hex(this.getSalt(encodedPassword) + password);
    }

    private static String getSalt(String encodedPassword) {
        return encodedPassword.split(DJANGO_SEPARATOR)[1];
    }

    private String getCandidateSHA1Hash(
            String encodedPassword, String password) {
        return DigestUtils.shaHex(this.getSalt(encodedPassword) + password);
    }

    private static String getAlgorithm(String encodedPassword) {
        return encodedPassword.split(DJANGO_SEPARATOR)[0];
    }

    private String findUserPassword(String username) throws Exception {
        return this.userDao.findPasswordByUsername(username);
    }
}
