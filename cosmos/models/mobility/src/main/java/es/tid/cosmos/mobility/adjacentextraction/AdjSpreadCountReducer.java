package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class AdjSpreadCountReducer extends Reducer<LongWritable,
        ProtobufWritable<TwoInt>, LongWritable, LongWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (ProtobufWritable<TwoInt> value : values) {
            valueCount++;
        }
        context.write(new LongWritable(0L), new LongWritable(valueCount));
    }
}
