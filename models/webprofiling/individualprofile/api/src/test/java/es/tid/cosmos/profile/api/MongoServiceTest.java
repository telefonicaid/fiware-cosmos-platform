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

package es.tid.cosmos.profile.api;

import org.junit.After;
import static org.junit.Assert.assertNotNull;
import org.junit.Before;
import org.junit.Test;

/**
 * @author sortega
 */
public class MongoServiceTest {
    private MongoService instance;

    @Before
    public void setUp() throws Exception {
        this.instance = new MongoService();
        this.instance.setHostname("localhost");
        this.instance.setDatabaseName("db1");
        this.instance.init();
    }

    @After
    public void tearDown() {
        this.instance.destroy();
    }

    @Test
    public void testGetDb() {
        assertNotNull(this.instance.getDb());
    }

    @Test
    public void testGetMongo() {
        assertNotNull(this.instance.getMongo());
    }
}
