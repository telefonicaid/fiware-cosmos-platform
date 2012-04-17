package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.MobilityMain;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.parsing.ClusterParser;
import es.tid.cosmos.mobility.util.Logger;

/**
 *
 * @author dmicol
 */
public class ClusterClientGetMinDistanceReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>,
        LongWritable, ProtobufWritable<Cluster>> {
    private static List<Cluster> clientClusters;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        if (clientClusters == null) {
            this.loadClientClusters(context.getConfiguration());
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

            for (Cluster cluster : clientClusters) {
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

    private void loadClientClusters(Configuration conf) throws IOException {
        FSDataInputStream in = null;
        BufferedReader br = null;
        try {
            FileSystem fs = FileSystem.get(conf);
            Path input = new Path(conf.get(MobilityMain.CENTROIDES_CLIENT_TAG));
            in = fs.open(input);
            br = new BufferedReader(new InputStreamReader(in));
            clientClusters = new LinkedList<Cluster>();
            String line;
            while ((line = br.readLine()) != null) {
                Cluster cluster = new ClusterParser(line).parse();
                clientClusters.add(cluster);
            }
        } catch (Exception ex) {
            clientClusters = null;
            Logger.get().fatal(ex);
            throw new IOException(ex);
        } finally {
            if (br != null) {
                try {
                    br.close();
                } catch (IOException ex) {
                }
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException ex) {
                }
            }
        }
    }
}
