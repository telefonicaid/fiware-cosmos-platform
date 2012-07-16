package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <NodeBts, ClusterVector>
 *
 * @author dmicol
 */
public class VectorNormalizedMapper extends Mapper<ProtobufWritable<NodeBts>,
        TypedProtobufWritable<ClusterVector>, ProtobufWritable<NodeBts>,
        TypedProtobufWritable<ClusterVector>> {
    @Override
    protected void map(ProtobufWritable<NodeBts> key,
            TypedProtobufWritable<ClusterVector> value, Context context)
            throws IOException, InterruptedException {
        ClusterVector.Builder vectorNormBuilder = ClusterVector.newBuilder();
        ClusterVector.Builder divBuilder = ClusterVector.newBuilder();
        final ClusterVector clusterVector = value.get();
        double sumvalues = 0D;
        for (int j = 0; j < clusterVector.getComsCount(); j++) {
            double elem = clusterVector.getComs(j);
            if (j < 24) {
                // Mondays, Tuesday, Wednesday and Thursday. Total: 103 days
                elem /= 103.0D;
            } else {
                // Friday, Saturdays or Sundays --> 26
                elem /= 26.0D;
            }
            sumvalues += elem;
            divBuilder.addComs(elem);
        }

        ClusterVector div = divBuilder.build();
        for (double comm : div.getComsList()) {
            double normCom = sumvalues == 0D ? comm : comm / sumvalues;
            vectorNormBuilder.addComs(normCom);
        }

        context.write(key, new TypedProtobufWritable<ClusterVector>(
                vectorNormBuilder.build()));
    }
}
