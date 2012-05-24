package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, PoiNew>
 * Output: <TwoInt, TwoInt>
 * 
 * @author dmicol
 */
public class AdjGroupTypePoiClientReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<PoiNew> poiNewList = new LinkedList<PoiNew>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final PoiNew poiNew = value.get().getPoiNew();
            poiNewList.add(poiNew);
        }
        
        for (PoiNew curPoi : poiNewList) {
            for (PoiNew tempPoi : poiNewList) {
                if (curPoi.getId() < tempPoi.getId()) {
                    context.write(TwoIntUtil.createAndWrap(curPoi.getBts(),
                                                           tempPoi.getBts()),
                                  MobDataUtil.createAndWrap(TwoIntUtil.create(
                                          curPoi.getId(), tempPoi.getId())));
                }
            }
        }
    }
}
