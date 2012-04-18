package es.tid.cosmos.mobility.data;

import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.RepeatedActivityAreas;

/**
 *
 * @author losa
 */
public abstract class RepeatedActivityAreasUtil implements ProtobufUtil {
    private static final String DELIMITER = "|";

    public static RepeatedActivityAreas create(Iterable<ActivityArea> areas) {
        RepeatedActivityAreas.Builder repActAreas =
            RepeatedActivityAreas.newBuilder()
                .addAllRepeatedActivityArea(areas);
        return repActAreas.build();
    }

    public static ProtobufWritable<RepeatedActivityAreas> wrap(
            RepeatedActivityAreas obj) {
        ProtobufWritable<RepeatedActivityAreas> wrapper =
            ProtobufWritable.newInstance(RepeatedActivityAreas.class);
        wrapper.set(obj);
        return wrapper;
    }

    public static ProtobufWritable<RepeatedActivityAreas> createAndWrap(
            Iterable<ActivityArea> allAreas) {
        return wrap(create(allAreas));
    }

    public static String toString(RepeatedActivityAreas obj) {
        // TODO: shouldn't this be a class, not List interface?
        List<ActivityArea> allAreas = obj.getRepeatedActivityAreaList();
        String ans = "";
        int pos = 0;
        for (ActivityArea area : allAreas) {
            pos += 1;
            ans += ActivityAreaUtil.toString(area);
            if (pos < allAreas.size()) {
                ans += DELIMITER;
            }
        }
        return ans;
    }

}
