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
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class CellUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(CellUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<Cell> wrapper = CellUtil.createAndWrap(
                1L, 2L, 3, 4, 5D, 6D);
        wrapper.setConverter(Cell.class);
        Cell obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getCellId());
        assertEquals(2L, obj.getBts());
        assertEquals(3, obj.getGeoloc1());
        assertEquals(4, obj.getGeoloc2());
        assertEquals(5D, obj.getPosx(), 0.0D);
        assertEquals(6D, obj.getPosy(), 0.0D);
    }
}
