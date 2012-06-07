package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.Cell;

/**
 *
 * @author dmicol
 */
public abstract class CellUtil {
    public static Cell create(long cellId, long bts, int geoLoc1,
                              int geoLoc2, double posX, double posY) {
        return Cell.newBuilder()
                .setCellId(cellId)
                .setBts(bts)
                .setGeoloc1(geoLoc1)
                .setGeoloc2(geoLoc2)
                .setPosx(posX)
                .setPosy(posY)
                .build();
    }

    public static ProtobufWritable<Cell> wrap(Cell obj) {
        ProtobufWritable<Cell> wrapper =
                ProtobufWritable.newInstance(Cell.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<Cell> createAndWrap(long cellId,
            long bts, int geoLoc1, int geoLoc2, double posX, double posY) {
        return wrap(create(cellId, bts, geoLoc1, geoLoc2, posX, posY));
    }
}
