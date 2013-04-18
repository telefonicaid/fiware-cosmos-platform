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

package es.tid.cosmos.injection.flume;

import java.util.Properties;
import java.util.UUID;

import org.junit.Before;
import org.junit.Test;

import static junit.framework.Assert.assertEquals;
import static junit.framework.Assert.assertNotNull;
import static junit.framework.Assert.assertNull;

/**
 *
 * @author apv
 */
public class StaticOpaqueTokenConfigProviderTest {

    private UUID opaqueToken;
    private String transformation;
    private String destination;
    private OpaqueTokenConfigProvider configProvider;

    @Before
    public void setUp() throws Exception {
        this.opaqueToken = UUID.randomUUID();
        this.transformation = "my-transformation";
        this.destination = "my-destination";

        OpaqueTokenConfigProvider.Builder builder =
                new StaticOpaqueTokenConfigProvider.Builder();
        Properties props = new Properties();
        props.setProperty(
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_TOKEN,
                this.opaqueToken.toString());
        props.setProperty(
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_TRANSFORMATION,
                this.transformation);
        props.setProperty(
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_DESTINATION,
                this.destination);
        this.configProvider = builder.newConfigProvider(props);
    }

    @Test
    public void testForToken() {
        OpaqueTokenConfig config = this.configProvider.forToken(
                this.opaqueToken);
        assertNotNull(config);
        assertEquals(this.transformation, config.getTransformation());
        assertEquals(this.destination, config.getDestinationPath());
    }

    @Test
    public void testForInvalidToken() {
        OpaqueTokenConfig config = this.configProvider.forToken(
                UUID.randomUUID());
        assertNull(config);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testBuilderInitWithMissingProperties() {
        OpaqueTokenConfigProvider.Builder builder =
                new StaticOpaqueTokenConfigProvider.Builder();
        Properties props = new Properties();
        props.setProperty(
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_TOKEN,
                this.opaqueToken.toString());
        this.configProvider = builder.newConfigProvider(props);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testBuilderInitWithInvalidToken() {
        OpaqueTokenConfigProvider.Builder builder =
                new StaticOpaqueTokenConfigProvider.Builder();
        Properties props = new Properties();
        props.setProperty(
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_TOKEN,
                "Hello world!");
        props.setProperty(
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_TRANSFORMATION,
                this.transformation);
        props.setProperty(
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_DESTINATION,
                this.destination);
        this.configProvider = builder.newConfigProvider(props);
    }


}
