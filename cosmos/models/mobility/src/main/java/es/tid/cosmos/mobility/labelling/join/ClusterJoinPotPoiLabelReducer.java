package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi|Long>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class ClusterJoinPotPoiLabelReducer extends Reducer<
        LongWritable, ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        List<Long> longList = new LinkedList<Long>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                case LONG:
                    longList.add(mobData.getLong());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        
        for (Long majPoiLbl : longList) {
            for (Poi potPoi : poiList) {
                if (majPoiLbl == potPoi.getLabelnodebts()) {
                    context.write(TwoIntUtil.createAndWrap(potPoi.getNode(),
                                                           potPoi.getBts()),
                                  MobDataUtil.createAndWrap(
                                          NullWritable.get()));
                }
            }
        }
    }
}
