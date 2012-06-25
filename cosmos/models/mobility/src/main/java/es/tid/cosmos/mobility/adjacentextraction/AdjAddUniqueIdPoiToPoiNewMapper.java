package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Counter;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi>
 * Output: <TwoInt, PoiNew>
 * 
 * @author dmicol
 */
class AdjAddUniqueIdPoiToPoiNewMapper extends Mapper<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Poi>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>> {

    Counter counter;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.counter = context.getCounter(Counters.COUNTER_FOR_POI_ID);
    }
    
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            TypedProtobufWritable<Poi> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final long id = this.counter.getValue();
        this.counter.increment(1L);
        final Poi poi = value.get();
        final PoiNew poiId = PoiNewUtil.create(
                (int) id, poi.getNode(), poi.getBts(),
                (poi.getConfidentnodebts() == 1) ?
                        poi.getLabelgroupnodebts() : 0,
                poi.getConfidentnodebts());
        ProtobufWritable<TwoInt> nodLbl = TwoIntUtil.createAndWrap(
                poi.getNode(), poiId.getLabelgroupnodebts());
        context.write(nodLbl, new TypedProtobufWritable<PoiNew>(poiId));
    }
}
