package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <TwoInt, Cluster>
 * 
 * @author dmicol
 */
public class ClusterClientBtsGetMinDistanceToClusterReducer extends Reducer<
        ProtobufWritable<NodeBts>, MobilityWritable<ClusterVector>,
        ProtobufWritable<TwoInt>, MobilityWritable<Cluster>> {
    private static List<Cluster> centroids = null;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        if (centroids == null) {
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
                double sumcom = 0D;
                double sumccom = 0D;
                double csumcom = 0D;
                double csumccom = 0D;
                double sumcomxccom = 0D;
                for (int nComs = 0; nComs < clusVector.getComsCount(); nComs++) {
                    double com = clusVector.getComs(nComs);
                    double ccom = cluster.getCoords().getComs(nComs);
                    sumcom += com;				// S1
                    sumccom += ccom;			        // S2
                    csumcom += com * com;		        // SC1
                    csumccom += ccom * ccom;	                // SC2
                    sumcomxccom += com * ccom;	                // S12
                }

                int nElem = clusVector.getComsCount();
                double dist = 1.0D - (((nElem * sumcomxccom) - (sumcom * sumccom))
                        / (Math.sqrt((nElem * csumcom) - (sumcom * sumcom))
                        * Math.sqrt((nElem * csumccom) - (sumccom * sumccom))));
                if (dist < mindist) {
                    mindist = dist;
                    minDistCluster = cluster;
                }
            }

            key.setConverter(NodeBts.class);
            final NodeBts nodeBts = key.get();
            ProtobufWritable<TwoInt> twoInt = TwoIntUtil.createAndWrap(
                    nodeBts.getUserId(), nodeBts.getBts());
            Cluster cluster = ClusterUtil.create(
                    minDistCluster.getLabel(), minDistCluster.getLabelgroup(),
                    mindist > minDistCluster.getDistance() ? 0 : 1,
                    0D, mindist, clusVector);
            context.write(twoInt, new MobilityWritable<Cluster>(cluster));
        }
    }
}
