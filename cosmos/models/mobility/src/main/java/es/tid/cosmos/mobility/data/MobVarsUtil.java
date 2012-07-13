package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;

/**
 *
 * @author losa
 */
public final class MobVarsUtil {

    private MobVarsUtil() {}

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

    public static String toString(MobVars obj, String separator) {
        return (obj.getNumPos() + separator + obj.getDifBtss() + separator +
                obj.getDifMuns() + separator + obj.getDifStates() + separator +
                obj.getMasscenterUtmx() + separator +
                obj.getMasscenterUtmy() + separator +
                obj.getRadius() + separator + obj.getDiamAreainf());
    }
}
