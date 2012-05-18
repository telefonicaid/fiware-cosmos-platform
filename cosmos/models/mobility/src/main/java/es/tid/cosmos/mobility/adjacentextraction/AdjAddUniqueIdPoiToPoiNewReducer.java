package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi>
 * Output: <TwoInt, PoiNew>
 * 
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToPoiNewReducer extends Reducer<
        LongWritable, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    private static final long MAX_NUM_PARTITIONS = 100L;
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        int hash = (int)(key.get() % MAX_NUM_PARTITIONS);
        int counter = 0;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final Poi poi = value.get().getPoi();
            PoiNew poiId = PoiNewUtil.create(
                    (int)(MAX_NUM_PARTITIONS * counter++) + hash,
                    poi.getNode(), poi.getBts(),
                    poi.getConfidentnodebts() == 1 ?
                            poi.getLabelgroupnodebts() : 0,
                    poi.getConfidentnodebts());
            ProtobufWritable<TwoInt> nodLbl = TwoIntUtil.createAndWrap(
                    poi.getNode(), poiId.getLabelgroupnodebts());
            context.write(nodLbl, MobDataUtil.createAndWrap(poiId));
        }
    }
}
