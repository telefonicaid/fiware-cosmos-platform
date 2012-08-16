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

import java.util.UUID;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;

/**
 * @author apv
 */
public class OpaqueTokenCacheTest {

    private OpaqueTokenCache cache;

    @Before
    public void setup() {
        this.cache = new OpaqueTokenCache(4);
    }

    @Test
    public void testCacheFailure() {
        UUID tk1 = UUID.randomUUID();
        UUID tk2 = this.cache.fetch(tk1.toString());
        assertEquals(tk1, tk2);
        assertEquals(1, this.cache.getSize());
    }

    @Test
    public void testCacheSuccess() {
        UUID tk1 = UUID.randomUUID();
        this.cache.fetch(tk1.toString()); // failure, fill cache
        UUID tk2 = this.cache.fetch(tk1.toString()); // success
        UUID tk3 = this.cache.fetch(tk1.toString()); // success again
        assertEquals(tk1, tk2);
        assertSame(tk2, tk3);
        assertEquals(1, this.cache.getSize());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testInvalidFetch() {
        this.cache.fetch("Hello world!");
    }

    @Test
    public void testReplacement() {
        for (int i = 0; i < this.cache.getCapacity() + 1; i++) {
            this.cache.fetch(UUID.randomUUID().toString());
        }
        assertEquals(this.cache.getCapacity(), this.cache.getSize());
    }
}
