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
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class NodeBtsDayUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(NodeBtsDayUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        long userId = 132L;
        long placeId = 81L;
        int workday = 5;
        int count = 3;
        ProtobufWritable<NodeBtsDay> wrapper = NodeBtsDayUtil.createAndWrap(
                userId, placeId, workday, count);
        wrapper.setConverter(NodeBtsDay.class);
        NodeBtsDay obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(userId, obj.getUserId());
        assertEquals(placeId, obj.getBts());
        assertEquals(workday, obj.getWorkday());
        assertEquals(count, obj.getCount());
    }
}
