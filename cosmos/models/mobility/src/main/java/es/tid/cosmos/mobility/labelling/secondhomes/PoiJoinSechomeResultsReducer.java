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
        Poi poi = null;
        TwoInt ioweekIowend = null;
        boolean hasSecHomeCount = false;
        int secHomeCount = 0;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    if (poi == null) {
                        poi = mobData.getPoi();
                    }
                    break;
                case TWO_INT:
                    if (ioweekIowend == null) {
                        ioweekIowend = mobData.getTwoInt();
                    }
                    break;
                case NULL:
                    hasSecHomeCount = true;
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
            
            if (poi != null && ioweekIowend != null && hasSecHomeCount) {
                // We already have all the required values, so there's no need
                // to process more records
                break;
            }
        }
        
        Poi.Builder outputPoi = Poi.newBuilder(poi);
        outputPoi.setInoutWeek((int)ioweekIowend.getNum1());
        outputPoi.setInoutWend((int)ioweekIowend.getNum2());
        if (hasSecHomeCount) {
            outputPoi.setLabelnodebts(100);
            outputPoi.setLabelgroupnodebts(100);
        }
        context.write(key, PoiUtil.wrap(outputPoi.build()));
    }
}
