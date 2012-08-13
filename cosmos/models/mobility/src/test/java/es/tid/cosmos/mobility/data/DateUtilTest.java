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
import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;

/**
 *
 * @author dmicol
 */
public class DateUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(DateUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<Date> wrapper = DateUtil.createAndWrap(2012, 4, 20, 5);
        Date obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(2012, obj.getYear());
        assertEquals(4, obj.getMonth());
        assertEquals(20, obj.getDay());
        assertEquals(5, obj.getWeekday());
    }

    @Test
    public void testParse() {
        Date obj = DateUtil.parse("2012|4|20|5", "\\|");
        assertEquals(2012, obj.getYear());
        assertEquals(4, obj.getMonth());
        assertEquals(20, obj.getDay());
        assertEquals(5, obj.getWeekday());
    }

    @Test
    public void testToString() {
        Date obj = DateUtil.create(2012, 4, 20, 5);
        assertEquals("2012|4|20|5", DateUtil.toString(obj, "|"));
    }

    @Test
    public void testCompare() {
        Date d1 = DateUtil.create(2012, 11, 20, 0);
        Date d2 = DateUtil.create(2013, 11, 20, 0);
        Date d3 = DateUtil.create(2012, 12, 20, 0);
        Date d4 = DateUtil.create(2012, 11, 21, 0);
        assertEquals( 0, DateUtil.compare(d1, d1));
        assertEquals( 0, DateUtil.compare(d2, d2));
        assertEquals(-1, DateUtil.compare(d1, d2));
        assertEquals( 1, DateUtil.compare(d2, d1));
        assertEquals(-1, DateUtil.compare(d1, d3));
        assertEquals( 1, DateUtil.compare(d3, d1));
        assertEquals(-1, DateUtil.compare(d1, d4));
        assertEquals( 1, DateUtil.compare(d4, d1));
    }
}
