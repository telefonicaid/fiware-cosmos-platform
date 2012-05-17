package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Cluster>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class VectorNodbtsOutReducer extends Reducer<ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>, NullWritable, Text> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final Cluster cluster = value.get().getCluster();
            String output =
                    TwoIntUtil.toString(twoInt) + ClusterUtil.DELIMITER
                    + cluster.getLabel() + ClusterUtil.DELIMITER
                    + cluster.getLabelgroup() + ClusterUtil.DELIMITER
                    + cluster.getConfident();
            for (double comm : cluster.getCoords().getComsList()) {
                output += ClusterUtil.DELIMITER + comm;
            }
            context.write(NullWritable.get(), new Text(output));
        }
    }
}
