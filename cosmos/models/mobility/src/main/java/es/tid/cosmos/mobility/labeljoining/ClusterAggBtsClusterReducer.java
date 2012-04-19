package es.tid.cosmos.mobility.labeljoining;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.PoiUtil;

/**
 *
 * @author dmicol
 */
public class ClusterAggBtsClusterReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<Poi>> {
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
                    throw new IllegalStateException();
            }
        }
        
        for (Poi poi : poiList) {
            for (Cluster cluster : clusterList) {
                Poi.Builder outputPoi = Poi.newBuilder(poi);
                outputPoi.setLabelbts(cluster.getLabel());
                outputPoi.setLabelgroupbts(cluster.getLabelgroup());
                outputPoi.setConfidentbts(cluster.getConfident());
                outputPoi.setDistancebts(cluster.getDistance());
                context.write(new LongWritable(poi.getBts()),
                              PoiUtil.wrap(outputPoi.build()));
            }
        }
    }
}
