package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Bts;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 * Input: <Long, Bts|Cluster>
 * Output: <Long, Cluster>
 * 
 * @author dmicol
 */
public class FilterBtsVectorReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    private static final double MAX_BTS_AREA = 10.83515D;
    private static final long MAX_COMMS_BTS = 70000L;
    
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Bts> btsList = new LinkedList<Bts>();
        List<Cluster> clusterList = new LinkedList<Cluster>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case BTS:
                    btsList.add(mobData.getBts());
                    break;
                case CLUSTER:
                    clusterList.add(mobData.getCluster());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        
        for (Bts bts : btsList) {
            for (Cluster cluster : clusterList) {
                int confident = cluster.getConfident();
                if (bts.getArea() <= MAX_BTS_AREA &&
                        bts.getComms() >= MAX_COMMS_BTS) {
                    confident = 1;
                }
                Cluster.Builder outputCluster = Cluster.newBuilder(cluster);
                outputCluster.setConfident(confident);
                context.write(key,
                              MobDataUtil.createAndWrap(outputCluster.build()));
            }
        }
    }
}
