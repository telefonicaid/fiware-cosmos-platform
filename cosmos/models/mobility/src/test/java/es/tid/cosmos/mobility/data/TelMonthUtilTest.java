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
import es.tid.cosmos.mobility.data.generated.MobProtocol.TelMonth;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class TelMonthUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(TelMonthUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<TelMonth> wrapper = TelMonthUtil.createAndWrap(1L, 2,
                                                                        true);
        TelMonth obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getPhone());
        assertEquals(2, obj.getMonth());
        assertEquals(true, obj.getWorkingday());
    }
}
