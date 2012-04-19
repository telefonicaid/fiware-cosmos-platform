package es.tid.cosmos.mobility.labeljoining;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.Poi;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.PoiUtil;

/**
 *
 * @author dmicol
 */
public class ClusterAggPotPoiPoisToPoiReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>, IntWritable,
        ProtobufWritable<Poi>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Poi> poiList = new LinkedList<Poi>();
        List<Cluster> clusterList = new LinkedList<Cluster>();
        List<Integer> intList = new LinkedList<Integer>();
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
                case INT:
                    intList.add(mobData.getInt());
                    break;
                default:
                    throw new IllegalStateException();
            }
        }
        
        if (poiList.size() != 1 || clusterList.size() != 1) {
            throw new IllegalStateException();
        }
        final Poi poi = poiList.get(0);
        Poi.Builder outputPoi = Poi.newBuilder(poi);
        if (!intList.isEmpty()) {
            outputPoi.setConfidentnodebts(1);
        }
        context.write(new IntWritable(outputPoi.getBts()),
                      PoiUtil.wrap(outputPoi.build()));
    }
}
