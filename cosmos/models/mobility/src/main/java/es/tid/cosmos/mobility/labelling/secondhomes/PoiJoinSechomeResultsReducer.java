package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;

/**
 *
 * @author dmicol
 */
public class PoiJoinSechomeResultsReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<Poi>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        List<TwoInt> twoIntList = new LinkedList<TwoInt>();
        int secHomeCount = 0;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                case TWO_INT:
                    twoIntList.add(mobData.getTwoInt());
                    break;
                case NULL:
                    secHomeCount++;
                    break;
                default:
                    throw new IllegalStateException();
            }
        }
        
        Poi poi = poiList.get(0);
        Poi.Builder outputPoi = Poi.newBuilder(poi);
        TwoInt ioweekIowend = twoIntList.get(0);
        outputPoi.setInoutWeek((int)ioweekIowend.getNum1());
        outputPoi.setInoutWend((int)ioweekIowend.getNum2());
        if (secHomeCount != 0) {
            outputPoi.setLabelnodebts(100);
            outputPoi.setLabelgroupnodebts(100);
        }
        context.write(key, PoiUtil.wrap(outputPoi.build()));
    }
}
