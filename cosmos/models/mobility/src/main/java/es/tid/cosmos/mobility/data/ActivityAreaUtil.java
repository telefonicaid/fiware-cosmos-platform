package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.util.Pair;

import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public abstract class ActivityAreaUtil implements ProtobufUtil {

    public static ActivityArea create(int numPos, int difBtss, int difMuns, 
            int difStates, double masscenterUtmX, double masscenterUtmY,
            double radius, double diamAreaInf) {
        return ActivityArea.newBuilder()
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

    public static ProtobufWritable<ActivityArea> createAndWrap(int numPos,
            int difBtss,  int difMuns, int difStates, double masscenterUtmX, 
            double masscenterUtmY, double radius, double diamAreaInf) {
        return wrap(create(numPos, difBtss, difMuns, difStates, masscenterUtmX,
                           masscenterUtmY, radius, diamAreaInf));
    }

    // TODO
    public static String toString(ProtobufWritable<ActivityArea> result) {
        throw new UnsupportedOperationException("Not yet implemented");
    }
}
