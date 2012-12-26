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

import org.apache.commons.cli.ParseException;
import org.apache.log4j.Level;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.util.Logger;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

/**
 * 
 * @author sortega
 */
public class ServerCommandLineTest extends BaseSftpTest {

    private static final org.apache.log4j.Logger LOGGER =
            Logger.get(ServerCommandLine.class);

    private ServerCommandLine instance;

    public ServerCommandLineTest() {
        super(LOGGER);
    }

    @Before
    public void setUp() throws Exception {
        this.instance = new ServerCommandLine();
    }

    @Test
    public void emptyCommandLine() throws Exception {
        this.instance.parse(new String[] {});
        assertFalse(this.instance.hasConfigFile());
    }

    @Test
    public void externalShortConfigCommandLine() throws Exception {
        this.instance.parse(new String[] {"-c", "/tmp/test.properties"});
        assertTrue(this.instance.hasConfigFile());
    }

    @Test
    public void externalLongConfigCommandLine() throws Exception {
        this.instance.parse(new String[] {"--config", "/tmp/test.properties"});
        assertTrue(this.instance.hasConfigFile());
    }

    @Test(expected=ParseException.class)
    public void unexpectedOptionsCommandLine() throws Exception {
        this.instance.parse(new String[] {"-x"});
    }
}
