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
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class TwoIntUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(TwoIntUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<TwoInt> wrapper = TwoIntUtil.createAndWrap(3L, 5L);
        wrapper.setConverter(TwoInt.class);
        TwoInt obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(3L, obj.getNum1());
        assertEquals(5L, obj.getNum2());
    }

    @Test
    public void testGetPartition() {
        TwoInt obj = TwoIntUtil.create(3L, 5L);
        assertEquals(1, TwoIntUtil.getPartition(obj, 2));
    }

    @Test
    public void testToString() {
        TwoInt obj = TwoIntUtil.create(3L, 5L);
        assertEquals("3|5", TwoIntUtil.toString(obj, "|"));
    }
}
