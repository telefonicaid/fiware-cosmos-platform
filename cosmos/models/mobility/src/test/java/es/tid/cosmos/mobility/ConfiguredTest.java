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

package es.tid.cosmos.mobility;

import java.io.InputStream;

import org.junit.Before;

import es.tid.cosmos.mobility.conf.MobilityConfiguration;

/**
 * Base class for tests that rely on sample configuration properties.
 *
 * @author sortega
 */
public class ConfiguredTest {

    public static final String MOBILITY_TEST_PROPERTIES =
            "/mobility.properties";
    private MobilityConfiguration mobilityConfiguration;

    @Before
    public void setUpConfig() throws Exception {
        InputStream configInput = MobilityConfiguration.class.getResource(
                MOBILITY_TEST_PROPERTIES).openStream();
        this.mobilityConfiguration = new MobilityConfiguration();
        this.mobilityConfiguration.load(configInput);
    }

    protected MobilityConfiguration getConf() {
        return this.mobilityConfiguration;
    }
}
