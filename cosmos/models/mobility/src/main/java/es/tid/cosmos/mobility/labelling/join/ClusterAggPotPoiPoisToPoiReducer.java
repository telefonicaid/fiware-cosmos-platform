package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Poi|Cluster|Null>
 * Output: <Long, Poi>
 * 
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToPoiReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        Poi poi = null;
        boolean hasNulls = false;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    if (poi == null) {
                        poi = mobData.getPoi();
                    }
                    break;
                case NULL:
                    hasNulls = true;
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
            if (poi != null && hasNulls) {
                break;
            }
        }
        if (poi == null) {
            return;
        }
        Poi.Builder outputPoi = Poi.newBuilder(poi);
        if (hasNulls) {
            outputPoi.setConfidentnodebts(1);
        }
        context.write(new LongWritable(outputPoi.getBts()),
                      MobDataUtil.createAndWrap(outputPoi.build()));
    }
}
