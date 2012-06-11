package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Cluster|Null>
 * Output: <TwoInt, Cluster>
 * 
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToClusterReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Message>,
        ProtobufWritable<TwoInt>, MobilityWritable<Cluster>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Cluster cluster = null;
        boolean hasNulls = false;
        for (MobilityWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Cluster && cluster == null) {
                cluster = (Cluster)message;
            } else if(message instanceof Null) {
                hasNulls = true;
            } else {
                throw new IllegalStateException("Unexpected input type: "
                        + message.getClass());
            }
            
            if (cluster != null && hasNulls) {
                break;
            }
        }
        if (cluster == null) {
            return;
        }
        Cluster.Builder outputCluster = Cluster.newBuilder(cluster);
        if (hasNulls) {
            outputCluster.setConfident(1);
        }
        context.write(key, new MobilityWritable<Cluster>(outputCluster.build()));
    }
}
