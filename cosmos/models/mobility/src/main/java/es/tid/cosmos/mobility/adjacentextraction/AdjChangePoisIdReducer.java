package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi|PoiNew>
 * Output: <TwoInt, Poi>
 * 
 * @author dmicol
 */
public class AdjChangePoisIdReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        List<PoiNew> poiNewList = new LinkedList<PoiNew>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                case POI_NEW:
                    poiNewList.add(mobData.getPoiNew());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        
        for (Poi poi : poiList) {
            for (PoiNew poiNew : poiNewList) {
                Poi.Builder outputPoiBuilder = Poi.newBuilder(poi);
                outputPoiBuilder.setId(poiNew.getId());
                context.write(key, MobDataUtil.createAndWrap(
                        outputPoiBuilder.build()));
            }
        }
    }
}
