package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ConvertClusterToMobDataReducer extends Reducer<LongWritable,
        ProtobufWritable<Cluster>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Cluster> value : values) {
            value.setConverter(Cluster.class);
            final Cluster cluster = value.get();
            context.write(key, MobDataUtil.createAndWrap(cluster));
        }
    }
}
