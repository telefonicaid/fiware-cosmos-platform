package es.tid.cosmos.mobility.outpois;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Cluster>
 * Output: <TwoInt, Cluster>
 * 
 * @author dmicol
 */
class PoiNormalizePoiVectorReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Cluster>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Cluster>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        List<Cluster> clusterList = new LinkedList<Cluster>();
        for (TypedProtobufWritable<Cluster> value : values) {
            clusterList.add(value.get());
        }
        if (clusterList.isEmpty()) {
            return;
        }

        final Cluster.Builder clusterSumBuilder = Cluster.newBuilder(
                clusterList.get(0));
        ClusterVector.Builder clusterSumCoordsBuilder =
                ClusterVector.newBuilder(clusterSumBuilder.getCoords());
        for (int i = 1; i < clusterList.size(); i++) {
            final Cluster cluster = clusterList.get(i);
            clusterSumCoordsBuilder.setComs(i,
                                            clusterSumCoordsBuilder.getComs(i)
                                            + cluster.getCoords().getComs(i));
        }
        clusterSumBuilder.setCoords(clusterSumCoordsBuilder);
        Cluster clusterSum = clusterSumBuilder.build();

        final Cluster.Builder clusterDivBuilder = Cluster.newBuilder(
                clusterSum);
        ClusterVector.Builder clusterDivCoordsBuilder =
                ClusterVector.newBuilder(clusterSum.getCoords());
        int sumValues = 0;
        for (int i = 0; i < clusterSum.getCoords().getComsCount(); i++) {
            double coms = clusterSum.getCoords().getComs(i);
            if (i < 24) {
                // Mondays, Tuesday, Wednesday and Thursday --> Total: 103 days
                coms /= 103.0D;
            } else {
                coms /= 26.0D;
            }
            sumValues += coms;
            clusterDivCoordsBuilder.addComs(coms);
        }
        clusterDivBuilder.setCoords(clusterDivCoordsBuilder);
        Cluster clusterDiv = clusterDivBuilder.build();

        final Cluster.Builder clusterNormBuilder = Cluster.newBuilder(
                clusterDiv);
        ClusterVector.Builder clusterNormCoordsBuilder =
                ClusterVector.newBuilder(clusterDiv.getCoords());
        for (int i = 0; i < clusterDiv.getCoords().getComsCount(); i++) {
            double coms = clusterDiv.getCoords().getComs(i);
            if (sumValues != 0) {
                coms /= sumValues;
            }
            clusterNormCoordsBuilder.addComs(coms);
        }
        clusterNormBuilder.setCoords(clusterNormCoordsBuilder);
        context.write(key,
                      new TypedProtobufWritable<Cluster>(clusterNormBuilder.build()));
    }
}
