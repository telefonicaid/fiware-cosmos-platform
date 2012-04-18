package es.tid.cosmos.mobility.data;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.RepeatedActivityAreas;

/**
 *
 * @author losa
 */
public abstract class RepeatedActivityAreasUtil implements ProtobufUtil {
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

}
