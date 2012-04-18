package es.tid.cosmos.mobility.clientbtslabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.PoiUtil;
import es.tid.cosmos.mobility.util.CentroidsCatalogue;

/**
 *
 * @author dmicol
 */
public class ClusterClientBtsGetMinDistanceToPoiReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>,
        LongWritable, ProtobufWritable<Poi>> {

    private static CentroidsCatalogue centroids = null;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        final Configuration conf = context.getConfiguration();
        if (centroids == null) {
            centroids = new CentroidsCatalogue(new Path(conf.get("centroids")),
                    conf);
        }
    }

    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        double mindist;
        double dist;
        double sumcom;
        double sumccom;
        double csumcom;
        double csumccom;
        double sumcomxccom;
        double com;
        double ccom;
        Cluster minDistCluster = null;

        for (ProtobufWritable<ClusterVector> value : values) {
            value.setConverter(ClusterVector.class);
            final ClusterVector clusVector = value.get();
            mindist = 1000;

            sumcom = sumccom = csumcom = csumccom = sumcomxccom = 0;
            for (Cluster cluster : centroids.getCentroids()) {
                for (int nComs = 0; nComs < clusVector.getComsCount(); nComs++) {
                    com = clusVector.getComs(nComs);
                    ccom = cluster.getCoords().getComs(nComs);
                    sumcom += com;				// S1
                    sumccom += ccom;			        // S2
                    csumcom += com * com;		        // SC1
                    csumccom += ccom * ccom;	                // SC2
                    sumcomxccom += com * ccom;	                // S12
                }

                int nElem = clusVector.getComsCount();
                dist = 1.0 - (((nElem * sumcomxccom) - (sumcom * sumccom))
                        / (Math.sqrt((nElem * csumcom) - (sumcom * sumcom))
                        * Math.sqrt((nElem * csumccom) - (sumccom * sumccom))));

                if (dist < mindist || minDistCluster == null) {
                    mindist = dist;
                    minDistCluster = cluster;
                }
            }
            mindist = Math.sqrt(mindist);

            key.setConverter(NodeBts.class);
            final NodeBts nodeBts = key.get();
            ProtobufWritable<Poi> poi = PoiUtil.createAndWrap(
                    0, nodeBts.getUserId(), nodeBts.getPlaceId(), 0, 0, 0, 0,
                    0, 0, 0, 0, minDistCluster.getLabel(),
                    minDistCluster.getLabelgroup(),
                    mindist > minDistCluster.getDistance() ? 0 : 1, mindist,
                    -1, -1);
            context.write(new LongWritable(nodeBts.getUserId()), poi);
        }
    }
}
