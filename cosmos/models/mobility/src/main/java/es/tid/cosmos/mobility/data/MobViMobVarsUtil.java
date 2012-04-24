package es.tid.cosmos.mobility.data;

import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;

/**
 *
 * @author losa
 */
public abstract class MobViMobVarsUtil {
    public static final String DELIMITER = "|";

    public static MobViMobVars create(Iterable<MobVars> areas) {
        MobViMobVars.Builder repActAreas = MobViMobVars.newBuilder()
                .addAllVars(areas);
        return repActAreas.build();
    }

    public static ProtobufWritable<MobViMobVars> wrap(MobViMobVars obj) {
        ProtobufWritable<MobViMobVars> wrapper =
            ProtobufWritable.newInstance(MobViMobVars.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<MobViMobVars> createAndWrap(
            Iterable<MobVars> allAreas) {
        return wrap(create(allAreas));
    }

    public static String toString(MobViMobVars obj) {
        List<MobVars> allAreas = obj.getVarsList();
        String ans = "";
        int pos = 0;
        for (MobVars area : allAreas) {
            pos += 1;
            ans += MobVarsUtil.toString(area);
            if (pos < allAreas.size()) {
                ans += DELIMITER;
            }
        }
        return ans;
    }

}
