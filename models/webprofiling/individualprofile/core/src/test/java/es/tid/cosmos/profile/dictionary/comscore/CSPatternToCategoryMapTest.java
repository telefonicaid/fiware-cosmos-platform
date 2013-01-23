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

package es.tid.cosmos.profile.dictionary.comscore;

import java.io.StringReader;

import static org.junit.Assert.assertArrayEquals;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.ExpectedException;

/**
 *
 * @author dmicol
 */
public class CSPatternToCategoryMapTest {
    private CSPatternToCategoryMap instance;
    private StringReader input;

    @Rule
    public ExpectedException thrown = ExpectedException.none();

    @Before
    public void setUp() throws Exception {
        this.instance = new CSPatternToCategoryMap();
        this.input = new StringReader("664169\n"
                + "670943\n"
                + "664169	778308	778309\n"
                + "664277	778308	3322210\n"
                + "666285	778308	3322210\n"
                + "670934	778213	778230	778314\n"
                + "670935	778227	778231\n"
                + "670937	778215	778248\n"
                + "670942	778227	778231\n"
                + "670943	778218	778230	778315\n");
        this.instance.init(this.input);
    }

    @Test
    public void shouldFindKnownMapping() throws Exception {
        assertArrayEquals(new long[]{ 778308L, 3322210L },
                this.instance.getCategories(666285L));
    }

    @Test()
    public void shouldFailForUnknownPatterns() throws Exception {
        this.thrown.expect(IllegalArgumentException.class);
        this.thrown.expectMessage("Unknown pattern ID: 555555");
        this.instance.getCategories(555555L);
    }

    @Test()
    public void shouldFailForOutOfRangePatterns() throws Exception {
        this.thrown.expect(IllegalArgumentException.class);
        this.thrown.expectMessage("Unable to cast value.");
        this.instance.getCategories(100000000000L);
    }
}
