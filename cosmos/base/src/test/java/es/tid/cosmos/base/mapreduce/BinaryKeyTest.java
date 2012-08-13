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

package es.tid.cosmos.base.mapreduce;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import org.junit.Test;

/**
 * Test case for BinaryKey
 *
 * @author dmicol
 */
public class BinaryKeyTest {
    @Test
    public void testEqualityAndHashCode() {
        CompositeKey b1 = new BinaryKey("keya", "1"),
                     b2 = new BinaryKey("keya", "1"),
                     b3 = new BinaryKey("1", "keya");
        assertFalse(b1.equals(new Object()));
        assertTrue(b1.equals(b2));
        assertEquals(b1.hashCode(), b2.hashCode());
        assertFalse(b1.equals(b3));
        assertEquals(b1.hashCode(), b3.hashCode());
    }

    @Test
    public void testGettersAndSetters() {
        BinaryKey b1 = new BinaryKey("keya", "1");
        assertEquals("keya", b1.getPrimaryKey());
        assertEquals("1", b1.getSecondaryKey());

        BinaryKey b2 = new BinaryKey();
        b2.setPrimaryKey("keya");
        b2.setSecondaryKey("1");

        assertTrue(b1.equals(b2));
    }

    @Test
    public void shouldBeOrderedByPrimaryKey() {
        CompositeKey a = new BinaryKey("keya", "3");
        CompositeKey b = new BinaryKey("keyb", "1");
        assertEquals(-1, a.compareTo(b));
        assertEquals( 1, b.compareTo(a));
        assertEquals( 0, a.compareTo(a));
    }

    @Test
    public void shouldBeOrderedBySecondaryKey() {
        CompositeKey a = new BinaryKey("keya", "3");
        CompositeKey a2 = new BinaryKey("keya", "2");
        assertEquals(1, a.compareTo(a2));
    }
}
