package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;

/**
 * Input: <LongWritable, Long>
 * Output: <LongWritable, Null>
 *
 * @author dmicol
 */
class AdjCountIndexesReducer extends Reducer <LongWritable,
        TypedProtobufWritable<Int64>, LongWritable, TypedProtobufWritable<Null>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Int64>> values, Context context)
            throws IOException, InterruptedException {
        long sum = 0L;
        for (TypedProtobufWritable<Int64> value : values) {
            sum += value.get().getValue();
        }
        context.getCounter(Counters.NUM_INDEXES).increment(sum);
        context.write(new LongWritable(sum),
                      new TypedProtobufWritable<Null>(Null.getDefaultInstance()));
    }
}
