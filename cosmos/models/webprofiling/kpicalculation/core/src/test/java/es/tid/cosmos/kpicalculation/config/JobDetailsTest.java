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

package es.tid.cosmos.kpicalculation.config;

import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;


/**
 *
 * @author dmicol, sortega
 */
public class JobDetailsTest {
    private KpiFeature instance;

    @Before
    public void setUp() {
        this.instance = new KpiFeature("a", new String[] { "b" });
    }

    @Test
    public void testGettersAndSetters() throws Exception {
        assertEquals("a", this.instance.getName());
        assertArrayEquals(new String[] { "b" }, this.instance.getFields());
        assertNull(this.instance.getGroup());
    }

    @Test
    public void testEquality() throws Exception {
        assertFalse(this.instance.equals(null));
        assertFalse(this.instance.equals(new Object()));
        assertFalse(this.instance.equals(new KpiFeature("different",
                new String[] { "b" })));
        assertFalse(this.instance.equals(new KpiFeature("a", new String[] {
            "different" })));
        assertFalse(this.instance.equals(new KpiFeature("a", new String[] {
            "b" }, "different")));
        assertEquals(this.instance, new KpiFeature("a", new String[] { "b" }));
    }
}
