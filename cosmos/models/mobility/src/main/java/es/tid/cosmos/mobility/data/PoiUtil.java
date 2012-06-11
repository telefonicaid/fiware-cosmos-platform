package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Poi;

/**
 *
 * @author dmicol
 */
public abstract class PoiUtil {
    public static final String DELIMITER = "|";
    
    public static Poi create(int id, long node, long bts, int labelnode,
            int labelgroupnode, int confidentnode, double distancenode,
            int labelbts, int labelgroupbts, int confidentbts,
            double distancebts, int labelnodebts, int labelgroupnodebts,
            int confidentnodebts, double distancenodebts, int inoutWeek,
            int inoutWend) {
        return Poi.newBuilder()
                .setId(id)
                .setNode(node)
                .setBts(bts)
                .setLabelnode(labelnode)
                .setLabelgroupnode(labelgroupnode)
                .setConfidentnode(confidentnode)
                .setDistancenode(distancenode)
                .setLabelbts(labelbts)
                .setLabelgroupbts(labelgroupbts)
                .setConfidentbts(confidentbts)
                .setDistancebts(distancebts)
                .setLabelnodebts(labelnodebts)
                .setLabelgroupnodebts(labelgroupnodebts)
                .setConfidentnodebts(confidentnodebts)
                .setDistancenodebts(distancenodebts)
                .setInoutWeek(inoutWeek)
                .setInoutWend(inoutWend)
                .build();
    }

    public static ProtobufWritable<Poi> wrap(Poi obj) {
        ProtobufWritable<Poi> wrapper = ProtobufWritable.newInstance(Poi.class);
        wrapper.set(obj);
        return wrapper;
    }
    
    public static ProtobufWritable<Poi> createAndWrap(int id, long node,
            long bts, int labelnode, int labelgroupnode, int confidentnode,
            double distancenode, int labelbts, int labelgroupbts,
            int confidentbts, double distancebts, int labelnodebts,
            int labelgroupnodebts, int confidentnodebts, double distancenodebts,
            int inoutWeek, int inoutWend) {
        return wrap(create(id, node, bts, labelnode, labelgroupnode,
                confidentnode, distancenode, labelbts, labelgroupbts,
                confidentbts, distancebts, labelnodebts, labelgroupnodebts,
                confidentnodebts, distancenodebts, inoutWeek, inoutWend));
    }
    
    public static String toString(Poi obj) {
        return (obj.getId() + DELIMITER + obj.getNode() + DELIMITER +
                obj.getBts() + DELIMITER + obj.getLabelnode() + DELIMITER +
                obj.getLabelgroupnode() + DELIMITER + obj.getConfidentnode() +
                DELIMITER + obj.getDistancenode() + DELIMITER +
                obj.getLabelbts() + DELIMITER + obj.getLabelgroupbts() +
                DELIMITER + obj.getConfidentbts() + DELIMITER +
                obj.getDistancebts() + DELIMITER + obj.getLabelnodebts() +
                DELIMITER + obj.getLabelgroupnodebts() + DELIMITER +
                obj.getConfidentnodebts() + DELIMITER +
                obj.getDistancenodebts() + DELIMITER + obj.getInoutWeek() +
                DELIMITER + obj.getInoutWend());
    }
}
