package es.tid.cosmos.kpicalculation;

import java.io.IOException;

import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.mapreduce.CompositeKey;

/**
 * This class performs the combining phase in the kpi aggregates grouped by
 * field calculation of the web profiling module. When the target aggregation is
 * a counter of unique values of a field of the items, depending on the values
 * of some other of its fields, this combiner will be used.
 *
 * Due to only unique keys will be taken into account during the reduction
 * phase, the local reduction of the data will consist on emit only the key with
 * a value of 1, independently of its existing the number of values.
 *
 * @author javierb
 */
public class KpiCounterByCombiner extends
        Reducer<CompositeKey, IntWritable, CompositeKey, IntWritable> {
    private static final IntWritable ONE = new IntWritable(1);

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
        context.write(key, ONE);
    }
}