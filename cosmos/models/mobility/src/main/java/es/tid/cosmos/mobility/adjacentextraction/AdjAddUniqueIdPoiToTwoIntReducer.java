package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntReducer extends Reducer<LongWritable,
        MobilityWritable<Poi>, LongWritable, MobilityWritable<TwoInt>> {
    private static final long MAX_NUM_PARTITIONS = 100L;
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        int hash = (int)(key.get() % MAX_NUM_PARTITIONS);
        int counter = 0;
        for (MobilityWritable<Poi> value : values) {
            int tableId = (int)(MAX_NUM_PARTITIONS * counter++) + hash;
            TwoInt poiPoimod = TwoIntUtil.create(tableId, tableId);
            context.write(new LongWritable(tableId), 
                          new MobilityWritable<TwoInt>(poiPoimod));
        }
    }
}
