package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 * Input: <Long, Cluster>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class VectorOneidOutReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, NullWritable, Text> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final Cluster cluster = value.get().getCluster();
            String output =
                    key.get() + ClusterUtil.DELIMITER
                    + cluster.getLabel() + ClusterUtil.DELIMITER
                    + cluster.getLabelgroup() + ClusterUtil.DELIMITER
                    + cluster.getConfident();
            for (double comm :cluster.getCoords().getComsList()) {
                 output += ClusterUtil.DELIMITER + comm;
            }
            context.write(NullWritable.get(), new Text(output));
        }
    }
}
