package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <NodeBts, ClusterVector>
 * 
 * @author dmicol
 */
public class VectorNormalizedReducer extends Reducer<ProtobufWritable<NodeBts>,
        MobilityWritable<ClusterVector>, ProtobufWritable<NodeBts>,
        MobilityWritable<ClusterVector>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<MobilityWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        ClusterVector.Builder vectorNormBuilder = ClusterVector.newBuilder();
        ClusterVector.Builder divBuilder = ClusterVector.newBuilder();
        for (MobilityWritable<ClusterVector> value : values) {
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
            
            context.write(key, new MobilityWritable<ClusterVector>(
                    vectorNormBuilder.build()));
        }
    }
}
