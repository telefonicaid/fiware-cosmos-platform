package es.tid.cosmos.mobility.labelling.client;

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
        for (ProtobufWritable<ClusterVector> value : values) {
            value.setConverter(ClusterVector.class);
            final ClusterVector clusterVector = value.get();
            double sumvalues = 0D;
            for (int j = 0; j < clusterVector.getComsCount(); j++) {
                double elem = clusterVector.getComs(j);
                if (j < 24) {
                    // Mondays, Tuesday, Wednesday and Thursday. Total: 121 days
                    elem /= 121.0D;
                } else {
                    elem /= 31.0D;
                }
                sumvalues += elem;
                divBuilder.addComs(elem);
            }

            ClusterVector div = divBuilder.build();
            for (double comm : div.getComsList()) {
                double normCom = sumvalues == 0D ? comm : comm / sumvalues;
                vectorNormBuilder.addComs(normCom);
            }
            
            ProtobufWritable<ClusterVector> vectorNorm =
                    ProtobufWritable.newInstance(ClusterVector.class);
            vectorNorm.set(vectorNormBuilder.build());
            context.write(key, vectorNorm);
        }
    }
}
