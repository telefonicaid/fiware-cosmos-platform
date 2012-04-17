package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.MobilityMain;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 *
 * @author dmicol
 */
public class ClusterClientGetMinDistanceReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>,
        LongWritable, ProtobufWritable<Cluster>> {
    private static CentroidsCatalogue centroids = null;
    
    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        final Configuration conf = context.getConfiguration();
        if (centroids == null) {
            centroids = new CentroidsCatalogue(
                    new Path(conf.get(MobilityMain.CENTROIDS_CLIENT_TAG)),
                    conf);
        }
    }

    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        double mindist;
        double dist;
        double com;
        double ccom;
        Cluster minDistCluster = null;

        for (ProtobufWritable<ClusterVector> value : values) {
            value.setConverter(ClusterVector.class);
            final ClusterVector clusVector = value.get();
            mindist = 1000;

            for (Cluster cluster : centroids.getCentroids()) {
                dist = 0;
                for (int nComs = 0; nComs < clusVector.getComsCount(); nComs++) {
                    ccom = cluster.getCoords().getComs(nComs);
                    com = clusVector.getComs(nComs);
                    dist += ((ccom - com) * (ccom - com));
                }
                if (dist < mindist || minDistCluster == null) {
                    mindist = dist;
                    minDistCluster = cluster;
                }
            }
            mindist = Math.sqrt(mindist);

            key.setConverter(NodeBts.class);
            final NodeBts nodeBts = key.get();
            ProtobufWritable<Cluster> outputCluster =
                    ClusterUtil.createAndWrap(
                            minDistCluster.getLabel(),
                            minDistCluster.getLabelgroup(),
                            mindist > minDistCluster.getDistance() ? 0 : 1,
                            0,
                            mindist,
                            ClusterVector.getDefaultInstance());
            context.write(new LongWritable(nodeBts.getUserId()), outputCluster);
        }
    }
}
