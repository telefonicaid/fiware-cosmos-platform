package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;

/**
 * Input: <LongWritable, Long>
 * Output: <LongWritable, Null>
 *
 * @author dmicol
 */
public class AdjCountIndexesReducer extends Reducer <LongWritable,
        MobilityWritable<Int64>, LongWritable, MobilityWritable<Null>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Int64>> values, Context context)
            throws IOException, InterruptedException {
        long sum = 0L;
        for (MobilityWritable<Int64> value : values) {
            sum += value.get().getNum();
        }
        context.getCounter(Counters.NUM_INDEXES).increment(sum);
        context.write(new LongWritable(sum),
                      new MobilityWritable<Null>(Null.getDefaultInstance()));
    }
}
