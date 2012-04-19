package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;

/**
 *
 * @author dmicol
 */
public class ExportClusterClientMinDistanceToTextReducer extends Reducer<
        LongWritable, ProtobufWritable<Cluster>, NullWritable, Text> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Cluster> value : values) {
            value.setConverter(Cluster.class);
            final Cluster cluster = value.get();
            context.write(NullWritable.get(),
                          new Text(key.get() + "|"
                                   + ClusterUtil.toString(cluster)));
        }
    }
}
