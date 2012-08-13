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
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static java.util.Arrays.asList;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author dmicol
 */
public class MobViMobVarsUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(MobViMobVarsUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        MobVars mobVars1 = MobVarsUtil.create(1, true, 2, 3, 4, 5, 6, 7, 8, 9);
        MobVars mobVars2 = MobVarsUtil.create(10, false, 20, 30, 40, 50, 60, 70,
                80, 90);
        ProtobufWritable<MobViMobVars> wrapper = MobViMobVarsUtil.createAndWrap(
                asList(mobVars1, mobVars2));
        wrapper.setConverter(MobViMobVars.class);
        final MobViMobVars obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(mobVars1, obj.getVars(0));
        assertEquals(mobVars2, obj.getVars(1));
    }
}
