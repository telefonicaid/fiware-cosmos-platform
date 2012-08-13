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
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Date;
import es.tid.cosmos.mobility.data.generated.BaseProtocol.Time;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class ItinTimeUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(ItinTimeUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ItinTime> wrapper = ItinTimeUtil.createAndWrap(
                Date.getDefaultInstance(), Time.getDefaultInstance(), 57L);
        wrapper.setConverter(ItinTime.class);
        ItinTime obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(Date.getDefaultInstance(), obj.getDate());
        assertEquals(Time.getDefaultInstance(), obj.getTime());
        assertEquals(57L, obj.getBts());
    }
}
