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
import es.tid.cosmos.mobility.data.generated.MobProtocol.MatrixRange;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class MatrixRangeUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(MatrixRangeUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<MatrixRange> wrapper = MatrixRangeUtil.createAndWrap(
                1L, 2L, 3L, 4, 5);
        wrapper.setConverter(MatrixRange.class);
        MatrixRange obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getNode());
        assertEquals(2L, obj.getPoiSrc());
        assertEquals(3L, obj.getPoiTgt());
        assertEquals(4, obj.getGroup());
        assertEquals(5, obj.getRange());
    }
}
