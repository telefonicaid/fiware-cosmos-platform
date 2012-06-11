package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <Long, Cluster>
 * 
 * @author dmicol
 */
public class ClusterClientGetMinDistanceReducer extends Reducer<
        ProtobufWritable<NodeBts>, MobilityWritable<ClusterVector>,
        LongWritable, MobilityWritable<Cluster>> {
    private static List<Cluster> centroids = null;
    
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (centroids == null) {
            final Configuration conf = context.getConfiguration();
            centroids = CentroidsCatalogue.load(new Path(conf.get("centroids")),
                                                conf);
        }
    }

    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<MobilityWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        for (MobilityWritable<ClusterVector> value : values) {
            final ClusterVector clusVector = value.get();
            double mindist = Double.POSITIVE_INFINITY;
            Cluster minDistCluster = null;
            for (Cluster cluster : centroids) {
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

            key.setConverter(NodeBts.class);
            final NodeBts nodeBts = key.get();
            Cluster outputCluster = ClusterUtil.create(
                            minDistCluster.getLabel(),
                            minDistCluster.getLabelgroup(),
                            mindist > minDistCluster.getDistance() ? 0 : 1,
                            0D,
                            mindist,
                            clusVector);
            context.write(new LongWritable(nodeBts.getUserId()),
                          new MobilityWritable<Cluster>(outputCluster));
        }
    }
}
