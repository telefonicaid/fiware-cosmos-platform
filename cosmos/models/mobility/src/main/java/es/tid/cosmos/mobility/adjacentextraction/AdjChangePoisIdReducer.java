package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;

/**
 *
 * @author dmicol
 */
public class AdjChangePoisIdReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<Poi>> {
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
                context.write(key, PoiUtil.wrap(outputPoiBuilder.build()));
            }
        }
    }
}
