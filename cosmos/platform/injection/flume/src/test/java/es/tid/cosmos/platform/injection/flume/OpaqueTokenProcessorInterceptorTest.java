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

package es.tid.cosmos.platform.injection.flume;

import java.nio.charset.Charset;
import java.util.LinkedList;
import java.util.List;
import java.util.Properties;
import java.util.UUID;

import org.apache.flume.Context;
import org.apache.flume.Event;
import org.apache.flume.conf.ConfigurationException;
import org.apache.flume.event.EventBuilder;
import org.apache.flume.interceptor.Interceptor;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 *
 * @author apv
 */
public class OpaqueTokenProcessorInterceptorTest {

    private static class CustomOpaqueTokenConfigProvider implements
            OpaqueTokenConfigProvider {

        @Override
        public OpaqueTokenConfig forToken(UUID opaqueToken) {
            return null;
        }

        public static class Builder implements OpaqueTokenConfigProvider.Builder {

            @Override
            public OpaqueTokenConfigProvider newConfigProvider(Properties props) {
                return new CustomOpaqueTokenConfigProvider();
            }
        }
    }

    private Context context;
    private Interceptor interceptor;
    private UUID opaqueToken;
    private Event inputEvent;
    private String destination;
    private String transformation;

    @Before
    public void setUp() throws Exception {
        this.opaqueToken = UUID.randomUUID();
        this.destination = "/home/apv/data";
        this.transformation = "tango-delta-mike";

        this.inputEvent = EventBuilder.withBody("Hello world!",
                Charset.forName("UTF-8"));

        Interceptor.Builder builder =
                new OpaqueTokenProcessorInterceptor.Builder();
        this.context = new Context();
        String prefix = OpaqueTokenProcessorInterceptor.Builder
                .PROPERTY_TOKEN_CONFIG_PROVIDER;
        this.context.put(prefix, OpaqueTokenConfigProviderType.STATIC.name());
        this.context.put(prefix + "." +
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_TOKEN,
                this.opaqueToken.toString());
        this.context.put(prefix + "." +
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_TRANSFORMATION,
                this.transformation);
        this.context.put(prefix + "." +
                StaticOpaqueTokenConfigProvider.Builder.PROPERTY_DESTINATION,
                this.destination);
        builder.configure(this.context);
        this.interceptor = builder.build();
        this.interceptor.initialize();
    }

    @After
    public void tearDown() {
        this.interceptor.close();
    }

    @Test
    public void testInterceptEvent() {
        this.inputEvent.getHeaders().put(
                OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN,
                this.opaqueToken.toString());
        Event outputEvent = this.interceptor.intercept(this.inputEvent);
        assertNotNull(outputEvent);
        assertEquals(this.destination, outputEvent.getHeaders().get(
                OpaqueTokenConstants.EVENT_HEADER_DESTINATION_PATH));
        assertEquals(this.transformation, outputEvent.getHeaders().get(
                OpaqueTokenConstants.EVENT_HEADER_TRANSFORMATION));
    }

    @Test
    public void testInterceptEventList() {
        List<Event> inputEvents = new LinkedList<Event>();
        for (int i = 0; i < 10; i++) {
            Event event = EventBuilder.withBody(this.inputEvent.getBody());
            event.getHeaders().put(
                    OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN,
                    this.opaqueToken.toString());
            inputEvents.add(event);
        }
        List<Event> outputEvents = this.interceptor.intercept(inputEvents);
        assertNotNull(outputEvents);
        assertEquals(10, outputEvents.size());
        for (Event event : outputEvents) {
            assertEquals(this.destination, event.getHeaders().get(
                    OpaqueTokenConstants.EVENT_HEADER_DESTINATION_PATH));
            assertEquals(this.transformation, event.getHeaders().get(
                    OpaqueTokenConstants.EVENT_HEADER_TRANSFORMATION));
        }
    }

    @Test
    public void testInterceptEventWithMissingToken() {
        Event outputEvent = this.interceptor.intercept(this.inputEvent);
        assertNull(outputEvent);
    }

    @Test
    public void testInterceptEventWithMalformedToken() {
        this.inputEvent.getHeaders().put(
                OpaqueTokenConstants.EVENT_HEADER_OPAQUE_TOKEN, "0123456789");
        Event outputEvent = this.interceptor.intercept(this.inputEvent);
        assertNull(outputEvent);
    }

    @Test(expected = ConfigurationException.class)
    public void testInitBuilderWithMissingConfigProvider() {
        Interceptor.Builder builder =
                new OpaqueTokenProcessorInterceptor.Builder();
        this.context.clear();
        builder.configure(this.context);
    }

    @Test(expected = ConfigurationException.class)
    public void testInitBuilderWithUnknownConfigProvider() {
        Interceptor.Builder builder =
                new OpaqueTokenProcessorInterceptor.Builder();
        this.context.put(OpaqueTokenProcessorInterceptor.Builder
                .PROPERTY_TOKEN_CONFIG_PROVIDER, "foobar");
        builder.configure(this.context);
    }

    @Test(expected = ConfigurationException.class)
    public void testInitBuilderWithInvalidConfigProviderClass() {
        Interceptor.Builder builder =
                new OpaqueTokenProcessorInterceptor.Builder();
        this.context.put(OpaqueTokenProcessorInterceptor.Builder
                .PROPERTY_TOKEN_CONFIG_PROVIDER, this.getClass().getName());
        builder.configure(this.context);
    }

    @Test
    public void testInitBuilderWithCustomConfigProvider() {
        Interceptor.Builder builder =
                new OpaqueTokenProcessorInterceptor.Builder();
        this.context.put(OpaqueTokenProcessorInterceptor.Builder
                .PROPERTY_TOKEN_CONFIG_PROVIDER,
                CustomOpaqueTokenConfigProvider.Builder.class.getName());
        builder.configure(this.context);
    }
}
