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
        double elem;
        for (ProtobufWritable<ClusterVector> value : values) {
            value.setConverter(ClusterVector.class);
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
            ProtobufWritable<ClusterVector> vectorNorm =
                    ProtobufWritable.newInstance(ClusterVector.class);
            vectorNorm.set(vectorNormBuilder.build());
            context.write(key, vectorNorm);
        }
    }
}
