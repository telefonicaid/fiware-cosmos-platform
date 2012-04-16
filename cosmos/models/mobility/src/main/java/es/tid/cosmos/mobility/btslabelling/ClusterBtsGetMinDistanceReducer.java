package es.tid.cosmos.mobility.btslabelling;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.parsing.BtsClustersParser;

/**
 *
 * @author dmicol
 */
public class ClusterBtsGetMinDistanceReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>,
        LongWritable, ProtobufWritable<Cluster>> {
    private static final String BTS_CLUSTERS_PATH = "/home/hdfs/bts_clusters";
    
    private static List<Cluster> btsClusters = null;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        loadBtsClusters(context.getConfiguration(), new Path(BTS_CLUSTERS_PATH));
    }
    
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        double mindist;
        double dist;
        int clusId = -1;
        double com;
        double ccom;
        for (ProtobufWritable<ClusterVector> value : values) {
            final ClusterVector clusVector = value.get();
            mindist = 1000000;
            for (int nClus = 0; nClus < btsClusters.size(); nClus++) {
                dist = 0;
                for (int nComs = 0; nComs < clusVector.getComsCount(); nComs++) {
                    ccom = btsClusters.get(nClus).getCoords().getComs(nComs);
                    com = clusVector.getComs(nComs);
                    dist += ((ccom - com) * (ccom - com));
                }
                if (dist < mindist) {
                    mindist = dist;
                    clusId = nClus;
                }
            }
            mindist = Math.sqrt(mindist);
            
            final NodeBts nodeBts = key.get();
            final Cluster cluster = btsClusters.get(clusId);
            ProtobufWritable<Cluster> outputCluster = ClusterUtil.createAndWrap(
                    cluster.getLabel(),
                    cluster.getLabelgroup(),
                    mindist > cluster.getMean() ? 0 : 1,
                    0,
                    cluster.getDistance(),
                    clusVector);
            context.write(new LongWritable(nodeBts.getPlaceId()), outputCluster);
        }
    }
    
    private static void loadBtsClusters(Configuration conf, Path input) {
        if (btsClusters != null) {
            return;
        }

        try {
            FileSystem fs = FileSystem.get(conf);
            FSDataInputStream in = fs.open(input);
            BufferedReader br = new BufferedReader(new InputStreamReader(in));
            btsClusters = new LinkedList<Cluster>();
            String line;
            while ((line = br.readLine()) != null) {
                BtsClustersParser parser = new BtsClustersParser(line);
                btsClusters.add(parser.parse());
            }
            in.close();
        } catch (IOException ex) {
            ex.printStackTrace(System.err);
        } catch (NullPointerException ex) {
            ex.printStackTrace(System.err);
        }
    }
}
