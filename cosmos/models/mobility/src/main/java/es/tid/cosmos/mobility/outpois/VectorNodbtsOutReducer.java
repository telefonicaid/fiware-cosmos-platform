package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorNodbtsOutReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<Cluster>, NullWritable, Text> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (ProtobufWritable<Cluster> value : values) {
            value.setConverter(Cluster.class);
            final Cluster cluster = value.get();
            context.write(NullWritable.get(), new Text(
                    TwoIntUtil.toString(twoInt) + "|"
                    + ClusterUtil.toString(cluster)));
        }
    }
}
