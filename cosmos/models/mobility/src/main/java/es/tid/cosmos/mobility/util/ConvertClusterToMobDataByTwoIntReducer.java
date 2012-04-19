package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ConvertClusterToMobDataByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<Cluster>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Cluster> value : values) {
            value.setConverter(Cluster.class);
            final Cluster cluster = value.get();
            context.write(key, MobDataUtil.createAndWrap(cluster));
        }
    }
}
