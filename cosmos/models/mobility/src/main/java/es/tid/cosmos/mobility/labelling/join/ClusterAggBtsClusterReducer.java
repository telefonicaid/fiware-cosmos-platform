package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 * Input: <Long, Poi|Cluster>
 * Output: <Long, Poi>
 * 
 * @author dmicol
 */
public class ClusterAggBtsClusterReducer extends Reducer<LongWritable,
        MobilityWritable<Message>, LongWritable, MobilityWritable<Poi>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = MobilityWritable.divideIntoTypes(values, Poi.class, Cluster.class);
        List<Poi> poiList = dividedLists.get(Poi.class);
        List<Cluster> clusterList = dividedLists.get(Cluster.class);
        
        for (Poi poi : poiList) {
            for (Cluster cluster : clusterList) {
                Poi.Builder outputPoi = Poi.newBuilder(poi);
                outputPoi.setLabelbts(cluster.getLabel());
                outputPoi.setLabelgroupbts(cluster.getLabelgroup());
                outputPoi.setConfidentbts(cluster.getConfident());
                outputPoi.setDistancebts(cluster.getDistance());
                context.write(new LongWritable(poi.getBts()),
                              new MobilityWritable<Poi>(outputPoi.build()));
            }
        }
    }
}
