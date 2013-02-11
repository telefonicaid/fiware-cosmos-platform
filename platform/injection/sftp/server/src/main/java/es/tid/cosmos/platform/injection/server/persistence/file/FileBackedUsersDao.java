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

package es.tid.cosmos.platform.injection.server.persistence.file;

import java.io.*;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.spec.InvalidKeySpecException;

import sun.reflect.generics.reflectiveObjects.NotImplementedException;

import es.tid.cosmos.platform.injection.server.auth.UsersDao;

/**
 * Data Access Layer for retrieve Public Key of Users
 * Reads the public key from user's home on system disk.
 *
 * @author ag453
 */
public class FileBackedUsersDao implements UsersDao {

    private final SshKeyReader sshKeyReader;

    public FileBackedUsersDao(SshKeyReader sshKeyReader) {
        this.sshKeyReader = sshKeyReader;
    }

    /**
     * This method returns password of user, but is not implemented
     * for File System persistence because is not needed for our application.
     *
     * @param username Username used on login process.
     * @return user's password
     */
    @Override
    public String findPasswordByUsername(String username) {
        throw new NotImplementedException();
    }

    /**
     * Read the public keys of user from his home directory
     * @param username Username used on login process.
     * @return Iterable of authorized public keys for this user.
     */
    @Override
    public Iterable<PublicKey> findPublicKeyByUsername(String username)
            throws IOException, InvalidKeySpecException,
                   NoSuchAlgorithmException {
        return this.sshKeyReader.readKeys(username);
    }
}
