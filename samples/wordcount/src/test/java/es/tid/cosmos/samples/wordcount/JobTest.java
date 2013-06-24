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

package es.tid.cosmos.samples.wordcount;

import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import static org.junit.Assert.assertTrue;

/**
 * Base class for job tests
 *
 * @author sortega
 */
public class JobTest {

    public static void assertMRChain(
            Job instance, Class<? extends Mapper> mapperClass,
            Class<? extends Reducer> reducerClass) throws Exception {
        assertTrue(
                "mapper class mismatch",
                instance.getMapperClass().isAssignableFrom(mapperClass));
        assertTrue(
                "reducer class mismatch",
                instance.getReducerClass().isAssignableFrom(reducerClass));
    }
}
