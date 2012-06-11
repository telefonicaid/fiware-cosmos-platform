package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.PoiNew;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, PoiNew>
 * Output: <TwoInt, TwoInt>
 * 
 * @author dmicol
 */
public class AdjGroupTypePoiClientReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<PoiNew>,
        ProtobufWritable<TwoInt>, MobilityWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<PoiNew>> values, Context context)
            throws IOException, InterruptedException {
        List<PoiNew> poiNewList = new LinkedList<PoiNew>();
        for (MobilityWritable<PoiNew> value : values) {
            final PoiNew poiNew = value.get();
            poiNewList.add(poiNew);
        }
        
        for (PoiNew curPoi : poiNewList) {
            for (PoiNew tempPoi : poiNewList) {
                if (curPoi.getId() < tempPoi.getId()) {
                    context.write(TwoIntUtil.createAndWrap(curPoi.getBts(),
                                                           tempPoi.getBts()),
                                  new MobilityWritable<TwoInt>(TwoIntUtil.create(
                                                  curPoi.getId(), tempPoi.getId())));
                }
            }
        }
    }
}
