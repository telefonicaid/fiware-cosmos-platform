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

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;

import com.sun.jersey.api.NotFoundException;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

/**
 * @author sortega
 */
public class TopCategoriesResourceTest {
    private TopCategoriesResource instance;
    private ProfileDAO profileMock;

    @Before
    public void setUp() throws Exception {
        this.instance = new TopCategoriesResource();
        this.profileMock = mock(ProfileDAO.class);
        this.instance.setDao(this.profileMock);
    }

    @Test
    public void shouldGetTheTopNAndNoMore() {
        when(this.profileMock.getLastCategoryMap("user1"))
                .thenReturn(new CategoryMap() {{
                    put("COMPUTING", 20L);
                    put("SPORTS", 1000L);
                    put("FASHION", 100L);
                    put("COMICS", 50L);
                    put("NEWS", 800L);
                }});

        int n = 3;
        String username = "user1";
        List expectedTopN = Arrays.asList(
                new HashMap() {{
                    put("category", "SPORTS");
                    put("count", 1000L);
                }},
                new HashMap() {{
                    put("category", "NEWS");
                    put("count", 800L);
                }},
                new HashMap() {{
                    put("category", "FASHION");
                    put("count", 100L);
                }});
        assertEquals(expectedTopN, this.instance.getTop(username, n));
    }

    @Test(expected=NotFoundException.class)
    public void shouldThrowExceptionOnUnknownUser() {
        when(this.profileMock.getLastCategoryMap("unknownUser"))
                .thenThrow(new IllegalArgumentException());
        this.instance.getTop("unknownUser", 3);
    }

    @Test(expected=NotFoundException.class)
    public void shouldThrowExceptionOnInvalidNumber() {
        when(this.profileMock.getLastCategoryMap("user1"))
                .thenReturn(new CategoryMap());
        this.instance.getTop("user1", -13);
    }
}
