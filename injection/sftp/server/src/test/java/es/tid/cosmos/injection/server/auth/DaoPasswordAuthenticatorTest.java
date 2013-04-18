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

import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.Mock;
import org.mockito.Mockito;
import org.mockito.runners.MockitoJUnitRunner;

/**
 * @author ag453
 */
@RunWith(MockitoJUnitRunner.class)
public class DaoPasswordAuthenticatorTest {

    @Mock
    private UsersDao userDao;
    private DaoPasswordAuthenticator instance;
    private static final String PASSWORD = "password";
    private static final String SHA1_PASSWORD = "sha1$a49dc$32c5cdb3e975dfb9c" +
            "1a4322bf57d471d10007aa5";
    private static final String MD5_PASSWORD = "md5$a49dc$767550276a632239710" +
            "caca535efc475";

    @Before
    public void setUp() throws Exception {
        this.instance = new DaoPasswordAuthenticator(this.userDao);
    }

    @Test
    public void authenticateMD5Passed() throws Exception {
        Mockito.when(this.userDao.findPasswordByUsername(
                Mockito.any(String.class))).thenReturn(MD5_PASSWORD);
        Assert.assertTrue(
                this.instance.authenticate("username", PASSWORD, null));
    }

    @Test
    public void authenticateSHA1Passed() throws Exception {
        Mockito.when(this.userDao.findPasswordByUsername(
                Mockito.any(String.class))).thenReturn(SHA1_PASSWORD);
        Assert.assertTrue(
                this.instance.authenticate("username", PASSWORD, null));
    }

    @Test
    public void authenticateMD5Faiis() throws Exception {
        Mockito.when(this.userDao.findPasswordByUsername(
                Mockito.any(String.class))).thenReturn(MD5_PASSWORD);
        Assert.assertFalse(
                this.instance.authenticate("username", "wrongPassword", null));
    }

    @Test
    public void authenticateSHA1Faiis() throws Exception {
        Mockito.when(this.userDao.findPasswordByUsername(
                Mockito.any(String.class))).thenReturn(SHA1_PASSWORD);
        Assert.assertFalse(
                this.instance.authenticate("username", "wrongPassword", null));
    }
}
