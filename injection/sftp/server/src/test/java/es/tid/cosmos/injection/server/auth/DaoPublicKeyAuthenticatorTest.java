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
import java.util.ArrayList;
import java.util.List;

import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.runners.MockitoJUnitRunner;

import static junit.framework.Assert.assertTrue;
import static junit.framework.Assert.assertFalse;

/**
 * @author ag453
 */
@RunWith(MockitoJUnitRunner.class)
public class DaoPublicKeyAuthenticatorTest {

    @Mock
    private UsersDao userDao;
    @Mock
    private PublicKey rsaPublicKey;
    @Mock
    private PublicKey dsaPublicKey;
    private DaoPublicKeyAuthenticator instance;

    private static final String RSA = "RSA";
    private static final String DSA = "DSA";

    @Before
    public void setUp() throws Exception {
        Mockito.when(this.rsaPublicKey.getAlgorithm()).thenReturn(RSA);
        Mockito.when(this.dsaPublicKey.getAlgorithm()).thenReturn(DSA);
        List<PublicKey> pks = new ArrayList<PublicKey>();
        pks.add(this.rsaPublicKey);
        Mockito.when(this.userDao.findPublicKeyByUsername(
                Mockito.any(String.class))).thenReturn(pks);
    }

    @Test
    public void authenticateTest() throws Exception {
        this.instance = new DaoPublicKeyAuthenticator(this.userDao);
        assertTrue(
                this.instance.authenticate("username",this.rsaPublicKey, null));
        assertFalse(
                this.instance.authenticate("username",this.dsaPublicKey, null));
    }
}
