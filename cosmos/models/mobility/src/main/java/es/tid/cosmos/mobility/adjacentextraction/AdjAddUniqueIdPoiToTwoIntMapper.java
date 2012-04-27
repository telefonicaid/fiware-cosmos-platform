package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToTwoIntMapper extends Mapper<
        ProtobufWritable<TwoInt>, ProtobufWritable<Poi>, LongWritable,
        ProtobufWritable<TwoInt>> {
    private static final long MAX_NUM_PARTITIONS = 100L;
    
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<Poi> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        Counter counter = context.getCounter(Counters.COUNTER_FOR_TABLE_ID);
        int hash = (int)TwoIntUtil.getPartition(nodBts, MAX_NUM_PARTITIONS);
        int tableId = (int)(MAX_NUM_PARTITIONS * counter.getValue()) + hash;
        counter.increment(1L);
        ProtobufWritable<TwoInt> poiPoimod = TwoIntUtil.createAndWrap(
                tableId, tableId);
        context.write(new LongWritable(tableId), poiPoimod);
    }
}
