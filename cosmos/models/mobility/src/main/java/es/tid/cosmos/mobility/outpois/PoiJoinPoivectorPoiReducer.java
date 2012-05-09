package es.tid.cosmos.mobility.outpois;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <TwoInt, ClusterVector|Poi>
 * Output: <TwoInt, Cluster>
 * 
 * @author dmicol
 */
public class PoiJoinPoivectorPoiReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<ClusterVector> clusterVectorList = new LinkedList<ClusterVector>();
        List<Poi> poiList = new LinkedList<Poi>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case CLUSTER_VECTOR:
                    clusterVectorList.add(mobData.getClusterVector());
                    break;
                case POI:
                    poiList.add(mobData.getPoi());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }
        
        key.setConverter(TwoInt.class);
        final TwoInt nodeBts = key.get();
        for (ClusterVector clusterVector : clusterVectorList) {
            for (Poi poi : poiList) {
                TwoInt.Builder outputNodeBtsBuilder = TwoInt.newBuilder(nodeBts);
                outputNodeBtsBuilder.setNum2(poi.getId());
                Cluster outputCluster = ClusterUtil.create(
                        poi.getLabelnodebts(), poi.getLabelgroupnodebts(),
                        poi.getConfidentnodebts(), 0.0D, 0.0D, clusterVector);
                context.write(TwoIntUtil.wrap(outputNodeBtsBuilder.build()),
                              MobDataUtil.createAndWrap(outputCluster));
            }
        }
    }
}
