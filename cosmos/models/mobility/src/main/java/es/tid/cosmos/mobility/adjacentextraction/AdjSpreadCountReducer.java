package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Int>
 *
 * @author dmicol
 */
class AdjSpreadCountReducer extends Reducer<
        LongWritable, TypedProtobufWritable<TwoInt>,
        LongWritable, TypedProtobufWritable<Int64>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        long valueCount = 0L;
        for (TypedProtobufWritable<TwoInt> value : values) {
            valueCount++;
        }
        context.write(new LongWritable(0L),
                      TypedProtobufWritable.create(valueCount));
    }
}
