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

package es.tid.cosmos.base.util;

import es.tid.cosmos.base.data.MessageUtil;
import org.junit.Test;

import java.security.NoSuchAlgorithmException;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;
import static org.junit.Assert.assertEquals;

/**
 *
 * @author dmicol
 */
public class SHAEncoderTest {

    @Test
    public void testUtilityClass() {
        assertUtilityClass(SHAEncoder.class);
    }

    @Test
    public void testEncode() throws NoSuchAlgorithmException {
        assertEquals("3fd75f795d13da104dd29ae84bd6116e050feff4e762dd07567613722"
                + "45fa7d4acdef8a8c0d2666d81d3e2da52dc4ba37656631bf136d80e3f926"
                + "35580482b2b", SHAEncoder.encode("432asdfAC"));
    }
}
