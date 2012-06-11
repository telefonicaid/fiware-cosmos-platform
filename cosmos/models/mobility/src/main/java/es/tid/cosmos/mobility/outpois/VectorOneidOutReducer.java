package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 * Input: <Long, Cluster>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class VectorOneidOutReducer extends Reducer<LongWritable,
        MobilityWritable<Cluster>, NullWritable, Text> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<Cluster> value : values) {
            final Cluster cluster = value.get();
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
