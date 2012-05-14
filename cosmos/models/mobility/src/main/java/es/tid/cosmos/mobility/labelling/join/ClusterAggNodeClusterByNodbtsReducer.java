package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Poi|Cluster>
 * Output: <TwoInt, Poi>
 * 
 * @author dmicol
 */
public class ClusterAggNodeClusterByNodbtsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        List<Cluster> clusterList = new LinkedList<Cluster>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                case CLUSTER:
                    clusterList.add(mobData.getCluster());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        
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
                              MobDataUtil.createAndWrap(outputPoi.build()));
            }
        }
    }
}
