package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Cluster|Null>
 * Output: <TwoInt, Cluster>
 * 
 * @author dmicol
 */
class ClusterAggPotPoiPoisToClusterReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Message>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Cluster>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Cluster cluster = null;
        boolean hasNulls = false;
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Cluster) {
                if (cluster == null) {
                    cluster = (Cluster) message;
                }
            } else if (message instanceof Null) {
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
        context.write(key, new TypedProtobufWritable<Cluster>(outputCluster.build()));
    }
}
