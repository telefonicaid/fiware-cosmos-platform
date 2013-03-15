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

package es.tid.cosmos.platform.injection.server;

import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.platform.injection.server.config.Configuration;

import static junit.framework.Assert.assertTrue;

/**
 * InjectionServerTest
 *
 * @author logc
 * @since 15/05/12
 */
public class InjectionServerTest extends BaseSftpTest {

    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(InjectionServer.class);

    private InjectionServer instance;

    public InjectionServerTest() {
        super(LOGGER);
    }

    @Before
    public void setUp() throws Exception {
        Configuration configuration = new Configuration(
                InjectionServerMain.class
                                   .getResource("/injection_server.dev.properties"));
        this.instance = new InjectionServer(configuration, null, null);
    }

    @Test
    public void testSetupSftpServer() throws Exception {
        assertTrue(this.instance instanceof InjectionServer);
        // No exceptions thrown
        this.instance.setupSftpServer();
    }
}
