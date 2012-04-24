package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 *
 * @author dmicol
 */
public class ClusterBtsGetMinDistanceReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>,
        LongWritable, ProtobufWritable<Cluster>> {
    private static CentroidsCatalogue centroids = null;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        if (centroids == null) {
            final Configuration conf = context.getConfiguration();
            centroids = new CentroidsCatalogue(new Path(conf.get("centroids")),
                                               conf);
        }
    }
    
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        for (ProtobufWritable<ClusterVector> value : values) {
            value.setConverter(ClusterVector.class);
            final ClusterVector clusVector = value.get();
            double mindist = Double.POSITIVE_INFINITY;
            Cluster minDistCluster = null;
            for (Cluster cluster : centroids.getCentroids()) {
                double dist = 0D;
                for (int nComs = 0; nComs < clusVector.getComsCount(); nComs++) {
                    double ccom = cluster.getCoords().getComs(nComs);
                    double com = clusVector.getComs(nComs);
                    dist += ((ccom - com) * (ccom - com));
                }
                if (dist < mindist) {
                    mindist = dist;
                    minDistCluster = cluster;
                }
            }
            mindist = Math.sqrt(mindist);

            ProtobufWritable<Cluster> outputCluster =
                    ClusterUtil.createAndWrap(
                            minDistCluster.getLabel(),
                            minDistCluster.getLabelgroup(),
                            mindist > minDistCluster.getMean() ? 0 : 1,
                            0D,
                            mindist,
                            clusVector);
            context.write(new LongWritable(nodeBts.getPlaceId()), outputCluster);
        }
    }
}
