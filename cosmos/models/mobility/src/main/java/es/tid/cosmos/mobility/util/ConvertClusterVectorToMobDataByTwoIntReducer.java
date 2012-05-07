package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ConvertClusterVectorToMobDataByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<ClusterVector>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<ClusterVector> value : values) {
            value.setConverter(ClusterVector.class);
            final ClusterVector clusterVector = value.get();
            context.write(key, MobDataUtil.createAndWrap(clusterVector));
        }
    }
}
