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

import static java.util.Arrays.asList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class NodeMxCounterUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(NodeMxCounterUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<NodeMxCounter> wrapper =
                NodeMxCounterUtil.createAndWrap(
                        asList(BtsCounter.getDefaultInstance(),
                               BtsCounter.getDefaultInstance()));
        NodeMxCounter obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(2, obj.getBtsCount());
    }
}
