package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, PoiNew>
 * Output: <Long, PoiNew>
 * 
 * @author dmicol
 */
class AdjSpreadPoisByPoiIdMapper extends Mapper<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<PoiNew>, LongWritable,
        TypedProtobufWritable<PoiNew>> {
    @Override
    protected void map(ProtobufWritable<TwoInt> key,
            TypedProtobufWritable<PoiNew> value, Context context)
            throws IOException, InterruptedException {
        final PoiNew poi = value.get();
        context.write(new LongWritable(poi.getId()), value);
    }
}
