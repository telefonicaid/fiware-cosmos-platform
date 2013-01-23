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

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 * This class makes the combining phase in the simple kpi aggregates calculation
 * of the web profiling module.When the target aggregation is a simple counter
 * of items depending on the values of some of its fields, this reducer will be
 * used.
 *
 * For any of the keys, all their values are aggregated to emit only one integer
 * value per key
 *
 * @author javierb
 */
public class KpiCounterCombiner extends
        Reducer<CompositeKey, IntWritable, CompositeKey, IntWritable> {
    private IntWritable counter;

    /**
     * Method that creates the objects that will be used during the reduce
     *
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        this.counter = new IntWritable();
    }

    /**
     * @param key
     *            is the key emitted by the mapper.
     * @param values
     *            are all the values aggregated during the mapping phase
     * @param context
     *            contains the context of the job run
     */
    @Override
    protected void reduce(CompositeKey key, Iterable<IntWritable> values,
            Context context) throws IOException, InterruptedException {
        int count = 0;
        while (values.iterator().hasNext()) {
            count += values.iterator().next().get();
        }
        this.counter.set(count);
        context.write(key, this.counter);
    }
}
