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

import java.io.IOException;
import java.net.URI;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.filecache.DistributedCache;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;

import es.tid.cosmos.profile.dictionary.Dictionary;

/**
 *
 * @author dmicol, sortega
 */
public final class DistributedCacheDictionary {
    private DistributedCacheDictionary() {
        // Utility class
    }

    public static final String LATEST_DICTIONARY =
            "/user/hdfs/comscore/latest/";
    private static final String DICTIONARY_NAMES =
            "comscore.dict.filenames";
    private static final String[] DEFAULT_DICTIONARY_NAMES = new String[] {
            "cs_terms_in_domain.bcp",
            "cs_mmxi.bcp.gz",
            "patterns_to_categories.txt",
            "cat_subcat_map.txt"
    };
    private static final int TERMS_PATH = 0;
    private static final int DICTIONARY_PATH = 1;
    private static final int CATEGORY_PATTERN_MAPPING_PATH = 2;
    private static final int CATEGORY_NAMES_PATH = 3;
    private static final String URI_SEPARATOR = "/";

    /**
     * Configure the distributed cache to share the dictionary files to all
     * the workers.
     *
     * @param job              job to configure
     * @param baseDirectory    dfs path to the dictionary files
     * @param dictionaryNames  name of the files to cache
     */
    public static void cacheDictionary(Job job, String baseDirectory,
            String[] dictionaryNames) {
        DistributedCache.createSymlink(job.getConfiguration());
        String prefix = baseDirectory;
        if (!prefix.endsWith(URI_SEPARATOR)) {
            prefix += URI_SEPARATOR;
        }
        for (String name : dictionaryNames) {
            DistributedCache.addCacheFile(URI.create(prefix + name),
                    job.getConfiguration());
        }
        job.getConfiguration().setStrings(DICTIONARY_NAMES, dictionaryNames);
    }

    /**
     * Configure the distributed cache to share the dictionary files to all
     * the workers.
     *
     * Dictionary files are assume to be named as in DEFAULT_DICTIONARY_NAMES
     *
     * @see DistributedCacheDictionary#DEFAULT_DICTIONARY_NAMES
     *
     * @param job              job to configure
     * @param baseDirectory    dfs path to the dictionary files
     */
    public static void cacheDictionary(Job job, String baseDirectory) {
        cacheDictionary(job, baseDirectory, DEFAULT_DICTIONARY_NAMES);
    }

    /**
     * Creates and initializes a dictionary from files allocated in the
     * distributed cache.
     *
     * Use in conjunction with #cacheDictionary
     *
     * @param context Hadoop context
     * @return        A dictionary
     * @throws IOException
     */
    public static Dictionary loadFromCache(Configuration config)
            throws IOException {
        List<String> cachedFiles = getCachedPaths(config);
        CSDictionary dictionary = new CSDictionary(
                cachedFiles.get(TERMS_PATH),
                cachedFiles.get(DICTIONARY_PATH),
                cachedFiles.get(CATEGORY_PATTERN_MAPPING_PATH),
                cachedFiles.get(CATEGORY_NAMES_PATH),
                CSDictionaryJNIInterface.DEFAULT_COMSCORE_LIBS);
        dictionary.init();
        return dictionary;
    }

    public static List<String> getCachedPaths(Configuration config)
            throws IOException {
        String[] fileNames = config.getStrings(DICTIONARY_NAMES,
                DEFAULT_DICTIONARY_NAMES);
        List<String> cachedPaths = new LinkedList<String>();
        for (String fileName : fileNames) {
            cachedPaths.add(getCachedPath(config, fileName));
        }
        return cachedPaths;
    }

    private static String getCachedPath(Configuration config, String fileName)
            throws IOException {
        for (Path path : DistributedCache.getLocalCacheFiles(config)) {
            if (path.getName().equals(fileName)) {
                return path.toString();
            }
        }
        throw new IllegalArgumentException("'" + fileName
                + "' is not in the distributed cache");
    }
}
