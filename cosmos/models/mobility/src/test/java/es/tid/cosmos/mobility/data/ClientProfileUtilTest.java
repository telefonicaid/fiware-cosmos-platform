
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
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;

/**
 *
 * @author ximo
 */
public class ClientProfileUtilTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(ClientProfileUtil.class);
    }

    @Test
    public void testCreateAndWrap() {
        ProtobufWritable<ClientProfile> wrapper =
                ClientProfileUtil.createAndWrap(1L, 2);
        wrapper.setConverter(ClientProfile.class);
        ClientProfile obj = wrapper.get();
        assertNotNull(obj);
        assertEquals(1L, obj.getUserId());
        assertEquals(2, obj.getProfileId());
    }
}
