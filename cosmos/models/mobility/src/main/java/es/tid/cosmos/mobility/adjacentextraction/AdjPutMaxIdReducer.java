package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class AdjPutMaxIdReducer extends Reducer<LongWritable,
        MobilityWritable<TwoInt>, LongWritable, MobilityWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt pairPois = null;
        long max = Long.MIN_VALUE;
        for (MobilityWritable<TwoInt> value : values) {
            pairPois = value.get();
            if (pairPois.getNum2() > max) {
                max = pairPois.getNum2();
            }
        }
        context.write(key, new MobilityWritable<TwoInt>(
                TwoIntUtil.create(pairPois.getNum1(), max)));
    }
}
