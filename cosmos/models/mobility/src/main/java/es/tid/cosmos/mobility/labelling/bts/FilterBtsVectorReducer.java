package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 * Input: <Long, Bts|Cluster>
 * Output: <Long, Cluster>
 * 
 * @author dmicol
 */
public class FilterBtsVectorReducer extends Reducer<LongWritable,
        MobilityWritable<Message>, LongWritable, MobilityWritable<Cluster>> {
    private double maxBtsArea;
    private int maxCommsBts;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final Configuration conf = context.getConfiguration();
        this.maxBtsArea = conf.getFloat(Config.MAX_BTS_AREA, Float.MAX_VALUE);
        this.maxCommsBts = conf.getInt(Config.MAX_COMMS_BTS, Integer.MAX_VALUE);
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = MobilityWritable.divideIntoTypes(
                values, Bts.class, Cluster.class);
        List<Bts> btsList = dividedLists.get(Bts.class);
        List<Cluster> clusterList = dividedLists.get(Cluster.class);
        
        for (Bts bts : btsList) {
            for (Cluster cluster : clusterList) {
                int confident = cluster.getConfident();
                if (bts.getComms() < this.maxCommsBts &&
                        bts.getArea() > this.maxBtsArea) {
                    confident = 0;
                }
                Cluster.Builder outputCluster = Cluster.newBuilder(cluster);
                outputCluster.setConfident(confident);
                context.write(key,
                              new MobilityWritable<Cluster>(outputCluster.build()));
            }
        }
    }
}
