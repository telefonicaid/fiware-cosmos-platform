package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Long>
 * 
 * @author dmicol
 */
public class AdjSpreadTableByPoiIdMapper extends Mapper<LongWritable,
        MobilityWritable<TwoInt>, LongWritable, MobilityWritable<Int64>> {
    @Override
    protected void map(LongWritable key,
            MobilityWritable<TwoInt> value, Context context)
            throws IOException, InterruptedException {
        final TwoInt poiPoiMod = value.get();
        context.write(new LongWritable(poiPoiMod.getNum1()),
                      MobilityWritable.create(poiPoiMod.getNum2()));
    }
}
