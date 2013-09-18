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

package es.tid.cosmos.injection.sftp;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.junit.Before;

/**
 * Suppress logging during SFTP tests execution. All warnings issued to
 * console are correctly caused by the tests, but they clutter the output
 * terminal.
 *
 * @author logc
 */
public class BaseSftpTest {

    private final Logger logger;

    public BaseSftpTest(Logger logger) {
        this.logger = logger;
    }

    @Before
    public void disableLogging() {
        this.logger.setLevel(Level.OFF);
    }
}
