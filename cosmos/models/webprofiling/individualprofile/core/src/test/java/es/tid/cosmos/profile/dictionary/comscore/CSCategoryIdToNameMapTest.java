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
import java.util.Collection;

import static org.hamcrest.Matchers.hasItems;
import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author dmicol
 */
public class CSCategoryIdToNameMapTest {
    private CSCategoryIdToNameMap instance;
    private StringReader input;

    @Before
    public void setUp() throws Exception {
        this.instance = new CSCategoryIdToNameMap();
        this.input = new StringReader(
                "3193263	Online Travel Agents	1	778234	\n"
                + "3260684	Career Resources	1	778216	\n"
                + "3260685	Job Search	1	778216	\n"
                + "3260686	Training and Education	1	778216	\n"
                + "3322148	Gambling	0	1	\n"
                + "3322199	Lotto/Sweepstakes	1	3322148	\n"
                + "3322200	Online Gambling	1	3322148	\n"
                + "3322210	Online Gaming	1	778308	\n"
                + "3357049	Information	1	778220	\n");
        this.instance.init(this.input);
    }

    @Test
    public void shouldFindKnownMapping() throws Exception {
        assertEquals("Training and Education",
                this.instance.getCategoryName(3260686));
    }

    @Test(expected=IllegalArgumentException.class)
    public void shouldFailForUnknownCategories() throws Exception {
        this.instance.getCategoryName(555555l);
    }

    @Test
    public void shouldCollectAllCategoryNames() throws Exception {
        final Collection<String> names = this.instance.getCategories();
        assertThat(names.size(), is(9));
        assertThat(names, hasItems("Online Gambling", "Training and Education",
                                   "Career Resources", "Job Search", "Gambling",
                                   "Online Travel Agents", "Information",
                                   "Online Gaming", "Lotto/Sweepstakes"));
    }
}
