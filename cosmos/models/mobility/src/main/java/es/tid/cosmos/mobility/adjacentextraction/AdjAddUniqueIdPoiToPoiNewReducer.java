package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjAddUniqueIdPoiToPoiNewReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<Poi>,
        ProtobufWritable<TwoInt>, ProtobufWritable<PoiNew>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<Poi>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt nodBts = key.get();
        int hash = (int)TwoIntUtil.getPartition(nodBts, 100L);
        int counter = 0;
        for (ProtobufWritable<Poi> value : values) {
            value.setConverter(Poi.class);
            final Poi poi = value.get();
            ProtobufWritable<TwoInt> nodLbl = TwoIntUtil.createAndWrap(
                    poi.getNode(), poi.getLabelgroupnodebts());
            ProtobufWritable<PoiNew> poiId = PoiNewUtil.createAndWrap(
                    (100 * counter++) + hash, poi.getNode(), poi.getBts(),
                    poi.getConfidentnodebts() == 1 ?
                            poi.getLabelgroupnodebts() : 0,
                    poi.getConfidentnodebts());
            context.write(nodLbl, poiId);
        }
    }
}
