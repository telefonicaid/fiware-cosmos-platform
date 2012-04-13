package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;

/**
 *
 * @author dmicol
 */
public class VectorNormalizedReducer extends Reducer<ProtobufWritable<NodeBts>,
        ProtobufWritable<ClusterVector>, ProtobufWritable<NodeBts>,
        ProtobufWritable<ClusterVector>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
                          Iterable<ProtobufWritable<ClusterVector>> values,
            Context context) throws IOException, InterruptedException {
        ClusterVector.Builder vectorNormBuilder = ClusterVector.newBuilder();
        ClusterVector.Builder divBuilder = ClusterVector.newBuilder();
        final NodeBts bts = key.get();
        double elem;
        for (ProtobufWritable<ClusterVector> value : values) {
            final ClusterVector clusterVector = value.get();
            double sumvalues = 0;

            for (int j = 0; j < clusterVector.getComsCount(); j++) {
                if (j < 24) {
                    // Mondays, Tuesday, Wednesday and Thursday. Total: 121 days
                    elem = clusterVector.getComs(j) / 121.0D;
                } else {
                    elem = clusterVector.getComs(j) / 31.0D;
                }
                sumvalues += elem;
                divBuilder.addComs(elem);
            }

            ClusterVector div = divBuilder.build();
            for (double comm : div.getComsList()) {
                elem = comm / sumvalues;
                vectorNormBuilder.addComs(elem);
            }
            ClusterVector vectorNorm = vectorNormBuilder.build();
            ProtobufWritable<ClusterVector> vectorNormWrapper =
                    ProtobufWritable.newInstance(ClusterVector.class);
            vectorNormWrapper.set(vectorNorm);
            context.write(key, vectorNormWrapper);
        }
    }
}
