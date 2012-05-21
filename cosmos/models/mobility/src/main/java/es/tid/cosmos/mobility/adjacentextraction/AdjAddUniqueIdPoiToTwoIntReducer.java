package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    private static final long MAX_NUM_PARTITIONS = 100L;
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        int hash = (int)(key.get() % MAX_NUM_PARTITIONS);
        int counter = 0;
        for (ProtobufWritable<MobData> value : values) {
            int tableId = (int)(MAX_NUM_PARTITIONS * counter++) + hash;
            TwoInt poiPoimod = TwoIntUtil.create(tableId, tableId);
            context.write(new LongWritable(tableId), 
                          MobDataUtil.createAndWrap(poiPoimod));
        }
    }
}
