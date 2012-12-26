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

package es.tid.cosmos.kpicalculation;

import org.apache.hadoop.io.IntWritable;
import static org.junit.Assert.assertEquals;
import org.junit.Before;
import org.junit.Test;

import es.tid.cosmos.base.mapreduce.BinaryKey;

/**
 *
 * @author dmicol
 */
public class KpiPartitionerTest {
    private KpiPartitioner instance;

    @Before
    public void setUp() {
        this.instance = new KpiPartitioner();
    }

    @Test
    public void shouldPartitionByFirstKey() {
        assertEquals(6, this.instance.getPartition(new BinaryKey("a", "b"),
                                                   new IntWritable(3), 7));
        assertEquals(6, this.instance.getPartition(new BinaryKey("a", "c"),
                                                   new IntWritable(2), 7));
    }
}
