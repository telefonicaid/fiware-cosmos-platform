package es.tid.cosmos.mobility.outpois;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, ClusterVector|Poi>
 * Output: <TwoInt, Cluster>
 * 
 * @author dmicol
 */
public class PoiJoinPoivectorPoiReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Message>,
        ProtobufWritable<TwoInt>, MobilityWritable<Cluster>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = MobilityWritable.divideIntoTypes(
                values, ClusterVector.class, Poi.class);
        List<ClusterVector> clusterVectorList = dividedLists.get(ClusterVector.class);
        List<Poi> poiList = dividedLists.get(Poi.class);
        
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
                              new MobilityWritable<Cluster>(outputCluster));
            }
        }
    }
}
