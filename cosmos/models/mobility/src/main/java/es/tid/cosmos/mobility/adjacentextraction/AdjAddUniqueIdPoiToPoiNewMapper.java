package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Maps and uuid and a Poi to the pair [node, labelgroupnodebts] and a PoiNew.
 *
 * Input:  <LongWritable, Poi>
 * Output: <TwoInt,       PoiNew>
 *
 * @author dmicol, sortega
 */
class AdjAddUniqueIdPoiToPoiNewMapper extends Mapper<
        LongWritable, TypedProtobufWritable<Poi>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>> {

    @Override
    protected void map(LongWritable uuidKey, TypedProtobufWritable<Poi> value,
            Context context) throws IOException, InterruptedException {
        final Poi poi = value.get();
        final PoiNew poiId = PoiNewUtil.create(
                uuidKey.get(), poi.getNode(), poi.getBts(),
                (poi.getConfidentnodebts() == 1) ?
                        poi.getLabelgroupnodebts() : 0,
                poi.getConfidentnodebts());
        ProtobufWritable<TwoInt> nodLbl = TwoIntUtil.createAndWrap(
                poi.getNode(), poiId.getLabelgroupnodebts());
        context.write(nodLbl, new TypedProtobufWritable<PoiNew>(poiId));
    }
}
