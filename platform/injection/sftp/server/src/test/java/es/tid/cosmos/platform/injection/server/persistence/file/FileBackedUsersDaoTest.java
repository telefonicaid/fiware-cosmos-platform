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

import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import java.security.PublicKey;
import java.security.spec.InvalidKeySpecException;
import java.util.ArrayList;
import java.util.List;

import com.google.common.collect.Iterables;
import junit.framework.Assert;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mockito;
import org.mockito.MockitoAnnotations.Mock;
import org.mockito.runners.MockitoJUnitRunner;

/**
 * FromFileUsersDaoImplTest
 *
 * @author ag453
 */
@RunWith(MockitoJUnitRunner.class)
public class FileBackedUsersDaoTest {

    @Mock
    private SshKeyReader sshKeyReader;
    @Mock
    private PublicKey publicKey;
    private FileBackedUsersDao instance;

    @Before
    public void setUp()
            throws InvalidKeySpecException, NoSuchAlgorithmException {
        List<PublicKey> keys = new ArrayList<PublicKey>();
        keys.add(this.publicKey);
        Mockito.when(this.sshKeyReader.readKeys(Mockito.anyString()))
                .thenReturn(keys);
        this.instance = new FileBackedUsersDao(this.sshKeyReader);
    }
    @Test
    public void findPublicKeysByUserNameTest()
            throws IOException, InvalidKeySpecException,
                   NoSuchAlgorithmException {
        Iterable<PublicKey> pks = this.instance.findPublicKeyByUsername("user");
        Assert.assertEquals(Iterables.size(pks), 1);
    }
}
