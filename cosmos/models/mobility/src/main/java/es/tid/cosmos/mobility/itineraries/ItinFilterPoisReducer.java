package es.tid.cosmos.mobility.itineraries;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ItinTime;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ItinFilterPoisReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<ItinTime> itinTimeList = new LinkedList<ItinTime>();
        List<Poi> poiList = new LinkedList<Poi>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case ITIN_TIME:
                    itinTimeList.add(mobData.getItinTime());
                    break;
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        key.setConverter(TwoInt.class);
        final TwoInt nodeBts = key.get();
        for (Poi poi : poiList) {
            for (ItinTime itinTime : itinTimeList) {
                ItinTime.Builder itinTimeOut = ItinTime.newBuilder(itinTime);
                itinTimeOut.setBts(poi.getId());
                ProtobufWritable<MobData> itinTimeOutWrapper =
                        MobDataUtil.createAndWrap(itinTimeOut.build());
                context.write(new LongWritable(nodeBts.getNum1()),
                              itinTimeOutWrapper);
            }
        }
    }
}
