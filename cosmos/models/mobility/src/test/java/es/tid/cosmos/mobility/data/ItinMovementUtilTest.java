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
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinMovement;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ItinTime;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class ItinMovementUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(ItinMovementUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ItinTime source = ItinTimeUtil.create(Date.getDefaultInstance(),
                                              Time.getDefaultInstance(), 57L);
        ItinTime target = ItinTimeUtil.create(Date.getDefaultInstance(),
                                              Time.getDefaultInstance(), 102L);
        ProtobufWritable<ItinMovement> wrapper = ItinMovementUtil.createAndWrap(
                source, target);
        wrapper.setConverter(ItinMovement.class);
        ItinMovement obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(source, obj.getSource());
        assertEquals(target, obj.getTarget());
    }
}
