package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi|Cluster>
 * Output: <TwoInt, Poi>
 * 
 * @author dmicol
 */
public class ClusterAggNodeClusterByNodbtsReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Poi>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedList = TypedProtobufWritable.groupByClass(
                values, Poi.class, Cluster.class);
        List<Poi> poiList = dividedList.get(Poi.class);
        List<Cluster> clusterList = dividedList.get(Cluster.class);
        
        for (Poi poi : poiList) {
            for (Cluster cluster : clusterList) {
                ProtobufWritable<TwoInt> nodbts = TwoIntUtil.createAndWrap(
                        poi.getNode(), poi.getBts());
                Poi.Builder outputPoi = Poi.newBuilder(poi);
                outputPoi.setLabelnode(cluster.getLabel());
                outputPoi.setLabelgroupnode(cluster.getLabelgroup());
                outputPoi.setConfidentnode(cluster.getConfident());
                outputPoi.setDistancenode(cluster.getDistance());
                context.write(nodbts,
                              new TypedProtobufWritable<Poi>(outputPoi.build()));
            }
        }
    }
}
