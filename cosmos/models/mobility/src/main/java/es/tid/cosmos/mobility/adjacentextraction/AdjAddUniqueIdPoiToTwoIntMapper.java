package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <LongWritable, TwoInt>
 * 
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntMapper extends Mapper<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> {
    private static final long MAX_NUM_PARTITIONS = 100L;
    
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        Counter counter = context.getCounter(Counters.COUNTER_FOR_TABLE_ID);
        int hash = (int)TwoIntUtil.getPartition(nodBts, MAX_NUM_PARTITIONS);
        int tableId = (int)(MAX_NUM_PARTITIONS * counter.getValue()) + hash;
        counter.increment(1L);
        TwoInt poiPoimod = TwoIntUtil.create(tableId, tableId);
        context.write(new LongWritable(tableId), 
                      MobDataUtil.createAndWrap(poiPoimod));
    }
}
