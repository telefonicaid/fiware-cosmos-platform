package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;

/**
 * Input: <Long, Bts|Cluster>
 * Output: <Long, Cluster>
 * 
 * @author dmicol
 */
class FilterBtsVectorReducer extends Reducer<
        LongWritable, TypedProtobufWritable<Message>,
        LongWritable, TypedProtobufWritable<Cluster>> {
    private double maxBtsArea;
    private int minCommsBts;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf =
                (MobilityConfiguration) context.getConfiguration();
        this.maxBtsArea = conf.getBtsMaxBtsArea();
        this.minCommsBts = conf.getBtsMinCommsBts();
    }
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, Bts.class, Cluster.class);
        List<Bts> btsList = dividedLists.get(Bts.class);
        List<Cluster> clusterList = dividedLists.get(Cluster.class);
        
        for (Bts bts : btsList) {
            for (Cluster cluster : clusterList) {
                int confident = cluster.getConfident();
                if (bts.getComms() < this.minCommsBts &&
                        bts.getArea() > this.maxBtsArea) {
                    confident = 0;
                }
                Cluster.Builder outputCluster = Cluster.newBuilder(cluster);
                outputCluster.setConfident(confident);
                context.write(key, new TypedProtobufWritable<Cluster>(
                        outputCluster.build()));
            }
        }
    }
}
