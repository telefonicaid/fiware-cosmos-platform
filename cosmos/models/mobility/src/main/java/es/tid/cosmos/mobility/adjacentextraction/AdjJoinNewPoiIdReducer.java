package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiNewUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjJoinNewPoiIdReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<PoiNew>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Long> longList = new LinkedList<Long>();
        List<PoiNew> poiNewList = new LinkedList<PoiNew>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            MobData mobData = value.get();
            switch (mobData.getType()) {
                case LONG:
                    longList.add(mobData.getLong());
                    break;
                case POI_NEW:
                    poiNewList.add(mobData.getPoiNew());
                    break;
                default:
                    throw new IllegalStateException();
            }
        }
        
        for (long poiMod : longList) {
            for (PoiNew poi : poiNewList) {
                PoiNew.Builder outputPoiBuilder = PoiNew.newBuilder(poi);
                outputPoiBuilder.setId(poi.getId());
                context.write(TwoIntUtil.createAndWrap(poi.getNode(),
                                                       poi.getBts()),
                              PoiNewUtil.wrap(outputPoiBuilder.build()));
            }
        }
    }
}
