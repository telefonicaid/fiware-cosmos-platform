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

import java.net.URI;
import static java.util.Arrays.asList;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.mapreduce.Job;
import static org.hamcrest.CoreMatchers.hasItem;
import static org.hamcrest.CoreMatchers.hasItems;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;

import static es.tid.cosmos.base.test.UtilityClassTest.assertUtilityClass;

/**
 *
 * @author sortega
 */
public class DistributedCacheDictionaryTest {
    private Job job;
    private Configuration config;

    @Before
    public void setUp() throws Exception {
        this.job = new Job();
        this.config = this.job.getConfiguration();
    }

    @Test
    public void testUtilityClass() {
        assertUtilityClass(DistributedCacheDictionary.class);
    }

    @Test
    public void testCacheDictionary() throws Exception {
        final String[] fileNames = new String[] { "a", "b", "c", "d" };
        DistributedCacheDictionary.cacheDictionary(this.job, "/base/path/",
                fileNames);
        assertArrayEquals(fileNames, this.config.getStrings(
                "comscore.dict.filenames"));
        assertThat(asList(DistributedCache.getCacheFiles(this.config)),
                hasItems(URI.create("/base/path/a"),
                         URI.create("/base/path/b"),
                         URI.create("/base/path/c"),
                         URI.create("/base/path/d")));
    }

    @Test
    public void shouldAddDirectorySeparator() throws Exception {
        DistributedCacheDictionary.cacheDictionary(this.job,
                "/base/path/without/trailingslash");
        assertThat(asList(DistributedCache.getCacheFiles(this.config)),
                   hasItem(URI.create("/base/path/without/trailingslash" +
                                      "/cs_terms_in_domain.bcp")));
    }

    @Test
    public void shouldHaveDefaultFileNames() throws Exception {
        DistributedCacheDictionary.cacheDictionary(this.job, "/base/path/");
        assertArrayEquals(new String[]{"cs_terms_in_domain.bcp",
                                       "cs_mmxi.bcp.gz",
                                       "patterns_to_categories.txt",
                                       "cat_subcat_map.txt"},
                this.config.getStrings("comscore.dict.filenames"));
    }

    @Test
    public void getCachedPathsTest() throws Exception {
        DistributedCacheDictionary.cacheDictionary(this.job, "/base/path/",
                new String[] { "a", "b", "c", "d" });
        config.set(DistributedCache.CACHE_LOCALFILES, "/local/path/d,"
                + "/local/path/b,/local/path/a,/local/path/c");
        assertEquals(asList("/local/path/a", "/local/path/b", "/local/path/c",
                "/local/path/d"), DistributedCacheDictionary.getCachedPaths(
                config));
    }
}
