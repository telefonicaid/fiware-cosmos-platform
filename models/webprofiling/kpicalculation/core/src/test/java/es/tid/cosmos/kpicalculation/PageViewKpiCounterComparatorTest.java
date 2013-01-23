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

package es.tid.cosmos.kpicalculation;

import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.CompositeKey;
import es.tid.cosmos.base.mapreduce.SingleKey;
import es.tid.cosmos.kpicalculation.data.KpiCalculationComparationException;

/**
 * @author dmicol, sortega
 */
public class PageViewKpiCounterComparatorTest extends BinaryComparatorBaseTest {
    private PageViewKpiCounterComparator instance;
    private CompositeKey k1;
    private CompositeKey k2;
    private CompositeKey k3;

    @Before
    public void setUp() {
        this.instance = new PageViewKpiCounterComparator();
        this.k1 = new SingleKey("a");
        this.k2 = new SingleKey("b");
        this.k3 = new SingleKey("a");
    }

    @Test
    public void testCompare() {
        assertEquals(-1, this.instance.compare(this.k1, this.k2));
        assertEquals( 1, this.instance.compare(this.k2, this.k1));
        assertEquals( 0, this.instance.compare(this.k1, this.k3));
    }

    @Test
    public void testCompareBytes() throws Exception {
        byte[] bytes1 = toByteArray(this.k1);
        byte[] bytes2 = toByteArray(this.k2);
        byte[] bytes3 = toByteArray(this.k3);
        assertEquals(-1, this.instance.compare(bytes1, 0, bytes1.length,
                                               bytes2, 0, bytes2.length));
        assertEquals( 1, this.instance.compare(bytes2, 0, bytes2.length,
                                               bytes1, 0, bytes1.length));
        assertEquals( 0, this.instance.compare(bytes1, 0, bytes1.length,
                                               bytes3, 0, bytes3.length));
    }

    @Test(expected=KpiCalculationComparationException.class)
    public void testCompareJunkBytes() throws Exception {
        byte[] bytes = new byte[] {1, 1, 2, 3, 5, 8, 13, 19};
        this.instance.compare(bytes, 0, bytes.length, bytes, 0, bytes.length);
    }
}
