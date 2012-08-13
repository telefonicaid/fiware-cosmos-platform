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

package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinRange;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class ItinRangeUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(ItinRangeUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ItinRange> wrapper = ItinRangeUtil.createAndWrap(
                1L, 2L, 3, 4, 5);
        wrapper.setConverter(ItinRange.class);
        ItinRange obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getPoiSrc());
        assertEquals(2L, obj.getPoiTgt());
        assertEquals(3, obj.getNode());
        assertEquals(4, obj.getGroup());
        assertEquals(5, obj.getRange());
    }
}
