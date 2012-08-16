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

package es.tid.cosmos.profile.api;

import java.util.List;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.assertEquals;
import static org.mockito.Mockito.*;

/**
 *
 * @author dmicol
 */
public class CategoriesTest {
    private ProfileDAO profile;

    @Before
    public void setUp() {
        CategoryMap map = new CategoryMap();
        map.put("B", 20L);
        map.put("A", 30L);
        map.put("C", 10L);
        map.put("D",  5L);
        this.profile = mock(ProfileDAO.class);
        when(profile.getLastCategoryMap(anyString())).thenReturn(map);
    }

    @Test
    public void testGetAll() {
        List results = Categories.getAll(this.profile, "abc");
        assertEquals(4, results.size());
    }

    @Test
    public void testGetTop() {
        List results = Categories.getTop(this.profile, "abc", 2);
        assertEquals(2, results.size());
    }

    @Test(expected=IllegalArgumentException.class)
    public void testGetTopWithNegativeN() {
        Categories.getTop(null, null, -1);
    }

    @Test(expected=IllegalArgumentException.class)
    public void testGetTopZero() {
        Categories.getTop(null, null, 0);
    }
}
