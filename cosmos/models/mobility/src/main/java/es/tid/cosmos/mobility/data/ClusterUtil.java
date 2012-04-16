package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;

/**
 *
 * @author sortega
 */
public abstract class ClusterUtil implements ProtobufUtil {
    public static Cluster create(int label, int labelgroup, int confident,
            double mean, double distance, ClusterVector coords) {
        Cluster.Builder cluster = Cluster.newBuilder()
                .setLabel(label)
                .setLabelgroup(labelgroup)
                .setConfident(confident)
                .setMean(mean)
                .setDistance(distance)
                .setCoords(coords);
        return cluster.build();
    }

    public static ProtobufWritable<Cluster> wrap(Cluster obj) {
        ProtobufWritable<Cluster> wrapper = ProtobufWritable.newInstance(
                Cluster.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<Cluster> createAndWrap(int label,
            int labelgroup, int confident, double mean, double distance,
            ClusterVector coords) {
        return wrap(create(label, labelgroup, confident, mean, distance, coords));
    }
}