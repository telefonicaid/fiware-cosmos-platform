package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.util.Pair;

import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public abstract class ActivityAreaUtil implements ProtobufUtil {
    private static final String DELIMITER = "|";

    public static ActivityArea create(int month, boolean isWorkDay, int numPos,
            int difBtss, int difMuns, int difStates, double masscenterUtmX,
            double masscenterUtmY, double radius, double diamAreaInf) {
        return ActivityArea.newBuilder()
            .setMonth(month)
            .setIsWorkDay(isWorkDay)
            .setNumPos(numPos)
            .setDifBtss(difBtss)
            .setDifMuns(difMuns)
            .setDifStates(difStates)
            .setMasscenterUtmX(masscenterUtmX)
            .setMasscenterUtmY(masscenterUtmY)
            .setRadius(radius)
            .setDiamAreaInf(diamAreaInf)
            .build();
    }

    public static ProtobufWritable<ActivityArea> wrap(ActivityArea obj) {
        ProtobufWritable<ActivityArea> wrapper =
                ProtobufWritable.newInstance(ActivityArea.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<ActivityArea> createAndWrap(int month,
            boolean isWorkDay, int numPos, int difBtss,  int difMuns,
            int difStates, double masscenterUtmX, double masscenterUtmY,
            double radius, double diamAreaInf) {
        return wrap(create(month, isWorkDay, numPos, difBtss, difMuns,
                           difStates, masscenterUtmX, masscenterUtmY, radius,
                           diamAreaInf));
    }

    public static String toString(ActivityArea obj) {
        return (obj.getNumPos() + DELIMITER + obj.getDifBtss() + DELIMITER +
                obj.getDifMuns() + DELIMITER + obj.getDifStates() + DELIMITER +
                obj.getMasscenterUtmX() + DELIMITER +
                obj.getMasscenterUtmY() + DELIMITER +
                obj.getRadius() + DELIMITER + obj.getDiamAreaInf());
    }
}
