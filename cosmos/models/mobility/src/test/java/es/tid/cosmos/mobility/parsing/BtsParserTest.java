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

package es.tid.cosmos.mobility.parsing;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 *
 * @author sortega
 */
public class BtsParserTest {

    @Test
    public void testParse() throws Exception {
        BtsParser parser = new BtsParser("173600|711.862|66737", "|");
        Bts bts = parser.parse();
        assertEquals(173600L, bts.getPlaceId());
        assertEquals(711.862D, bts.getArea(), 0.0D);
        assertEquals(66737L, bts.getComms());
    }
}
