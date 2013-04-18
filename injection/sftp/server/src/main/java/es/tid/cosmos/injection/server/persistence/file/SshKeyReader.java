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

package es.tid.cosmos.injection.server.persistence.file;

import java.security.PublicKey;
import java.util.List;

/**
 * This class make basic operations with files
 *
 * @author ag453
 */
public interface SshKeyReader {

    /**
     * Read User PublicKeys from a remote.
     *
     * @param username
     * @return
     */
    List<PublicKey> readKeys(String username);
}
