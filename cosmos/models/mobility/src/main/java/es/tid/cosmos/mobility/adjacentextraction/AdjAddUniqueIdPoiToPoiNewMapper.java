package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <TwoInt, Poi>
 * Output: <TwoInt, PoiNew>
 * 
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToPoiNewMapper extends Mapper<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    private static final long MAX_NUM_PARTITIONS = 100L;
    
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        Counter counter = context.getCounter(Counters.COUNTER_FOR_POI_ID);
        int hash = (int)TwoIntUtil.getPartition(nodBts, MAX_NUM_PARTITIONS);
        value.setConverter(MobData.class);
        final Poi poi = value.get().getPoi();
        PoiNew poiId = PoiNewUtil.create(
                (int)(MAX_NUM_PARTITIONS * counter.getValue()) + hash,
                poi.getNode(), poi.getBts(),
                poi.getConfidentnodebts() == 1 ?
                        poi.getLabelgroupnodebts() : 0,
                poi.getConfidentnodebts());
        counter.increment(1L);
        ProtobufWritable<TwoInt> nodLbl = TwoIntUtil.createAndWrap(
                poi.getNode(), poiId.getLabelgroupnodebts());
        context.write(nodLbl, MobDataUtil.createAndWrap(poiId));
    }
}
