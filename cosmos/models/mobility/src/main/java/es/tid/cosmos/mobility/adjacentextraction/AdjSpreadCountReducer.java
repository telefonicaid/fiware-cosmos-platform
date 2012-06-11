package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Int>
 * 
 * @author dmicol
 */
public class AdjSpreadCountReducer extends Reducer<LongWritable,
        MobilityWritable<TwoInt>, LongWritable, MobilityWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (MobilityWritable<TwoInt> value : values) {
            valueCount++;
        }
        context.write(new LongWritable(0L),
                      MobilityWritable.create(valueCount));
    }
}
