package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi|PoiNew>
 * Output: <TwoInt, Poi>
 *
 * @author dmicol
 */
class AdjChangePoisIdReducer extends Reducer<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Poi>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> typeLists = TypedProtobufWritable.groupByClass(
                values, Poi.class, PoiNew.class);
        final List<Poi> poiList = typeLists.get(Poi.class);
        final List<PoiNew> poiNewList = typeLists.get(PoiNew.class);
        
        for (Poi poi : poiList) {
            for (PoiNew poiNew : poiNewList) {
                Poi.Builder outputPoiBuilder = Poi.newBuilder(poi);
                outputPoiBuilder.setId(poiNew.getId());
                context.write(key, new TypedProtobufWritable<Poi>(
                        outputPoiBuilder.build()));
            }
        }
    }
}
