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
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.annotation.Nullable;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.platform.injection.server.auth.PublicKeyUtils;

/**
 * Read User PublicKeys from a local file System.
 *
 * @author AG453
 */
public class LocalFileSshKeyReader implements SshKeyReader {

    private static final String USERNAME_STRING_PATTERN = "^[a-z0-9_-]{3,15}$";
    private static final Pattern USERNAME_PATTERN =
            Pattern.compile(USERNAME_STRING_PATTERN);
    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(LocalFileSshKeyReader.class);

    @Override
    public List<PublicKey> readKeys(String username) {
        File authorizedKeys = this.getAuthorizedKeysFile(username);
        return this.getPublicKeys(authorizedKeys);
    }

    @Nullable
    private List<PublicKey> getPublicKeys(File authorizedKeys) {
        BufferedReader bufferedReader;
        try {
            bufferedReader = new BufferedReader(
                    new FileReader(authorizedKeys));
        } catch (FileNotFoundException e) {
            LOGGER.warn("authorized_keys file not found");
            return null;
        }
        try {
            return this.readKeys(bufferedReader);
        } catch (IOException e) {
            LOGGER.error("Error reading authorized_keys file");
            return null;
            }
    }

    private List<PublicKey> readKeys(BufferedReader bufferedReader)
            throws IOException {
        List<PublicKey> keys = new ArrayList<PublicKey>();
        String line;
        while ((line = bufferedReader.readLine()) != null) {
            try {
                if (!isCommentLine(line)) {
                    keys.add(PublicKeyUtils.decodePublicKey(line));
                }
            } catch (NoSuchAlgorithmException e) {
                LOGGER.warn(String.format(
                        "Key algorithm not supported on key " + "'%1$s', %2$s",
                        line, e.getMessage()));
            } catch (InvalidKeySpecException e) {
                LOGGER.warn(String.format("Wrong key '%1$s', %2$s", line,
                        e.getMessage()));
            }
        }
        return keys;
    }

    @Nullable
    private File getAuthorizedKeysFile(String username) {
        if (!isValidUsername(username)) {
            return null;
        }
        String path = String.format("/home/%1$s/.ssh/authorized_keys", username);
        File authorizedKeys = new File(path);
        if (!authorizedKeys.exists()) {
            return null;
        }
        return authorizedKeys;
    }

    private static boolean isValidUsername(String username) {
        Matcher matcher = USERNAME_PATTERN.matcher(username);
        return matcher.matches();
    }

    private static boolean isCommentLine(String line) {
        return line.trim().startsWith("#");
    }
}
