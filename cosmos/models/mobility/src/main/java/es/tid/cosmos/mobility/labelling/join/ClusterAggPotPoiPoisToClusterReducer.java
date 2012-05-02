package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToClusterReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<Cluster>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        Cluster cluster = null;
        boolean hasNulls = false;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    break;
                case CLUSTER:
                    if (cluster == null) {
                        cluster = mobData.getCluster();
                    }
                    break;
                case NULL:
                    hasNulls = true;
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
            if (cluster != null && hasNulls) {
                break;
            }
        }
        
        Cluster.Builder outputCluster = Cluster.newBuilder(cluster);
        if (hasNulls) {
            outputCluster.setConfident(1);
        }
        context.write(key, ClusterUtil.wrap(outputCluster.build()));
    }
}
