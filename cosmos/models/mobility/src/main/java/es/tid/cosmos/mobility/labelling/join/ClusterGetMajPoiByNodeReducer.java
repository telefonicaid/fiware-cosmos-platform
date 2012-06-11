package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Long>
 * 
 * @author dmicol
 */
public class ClusterGetMajPoiByNodeReducer extends Reducer<LongWritable,
        MobilityWritable<TwoInt>, LongWritable, MobilityWritable<Int64>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt maxPoiblCount = null;
        for (MobilityWritable<TwoInt> value : values) {
            final TwoInt poilblCount = value.get();
            if (maxPoiblCount == null ||
                    poilblCount.getNum2() > maxPoiblCount.getNum2()) {
                maxPoiblCount = poilblCount;
            }
        }
        context.write(key, MobilityWritable.create(maxPoiblCount.getNum1()));
    }
}
