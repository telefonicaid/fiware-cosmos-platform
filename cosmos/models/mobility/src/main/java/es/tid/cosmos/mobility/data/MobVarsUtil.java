package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.util.Pair;

import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public abstract class MobVarsUtil implements ProtobufUtil {
    private static final String DELIMITER = "|";

    public static MobVars create(int month, boolean isWorkDay, int numPos,
            int difBtss, int difMuns, int difStates, double masscenterUtmX,
            double masscenterUtmY, double radius, double diamAreaInf) {
        return MobVars.newBuilder()
            .setMonth(month)
            .setWorkingday(isWorkDay)
            .setNumPos(numPos)
            .setDifBtss(difBtss)
            .setDifMuns(difMuns)
            .setDifStates(difStates)
            .setMasscenterUtmx(masscenterUtmX)
            .setMasscenterUtmy(masscenterUtmY)
            .setRadius(radius)
            .setDiamAreainf(diamAreaInf)
            .build();
    }

    public static ProtobufWritable<MobVars> wrap(MobVars obj) {
        ProtobufWritable<MobVars> wrapper =
                ProtobufWritable.newInstance(MobVars.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MobVars> createAndWrap(int month,
            boolean isWorkDay, int numPos, int difBtss,  int difMuns,
            int difStates, double masscenterUtmX, double masscenterUtmY,
            double radius, double diamAreaInf) {
        return wrap(create(month, isWorkDay, numPos, difBtss, difMuns,
                           difStates, masscenterUtmX, masscenterUtmY, radius,
                           diamAreaInf));
    }

    public static String toString(MobVars obj) {
        return (obj.getNumPos() + DELIMITER + obj.getDifBtss() + DELIMITER +
                obj.getDifMuns() + DELIMITER + obj.getDifStates() + DELIMITER +
                obj.getMasscenterUtmx() + DELIMITER +
                obj.getMasscenterUtmy() + DELIMITER +
                obj.getRadius() + DELIMITER + obj.getDiamAreainf());
    }
}
