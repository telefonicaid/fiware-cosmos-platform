package es.tid.cosmos.mobility.activityarea;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.ActivityAreaKeyUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;

/**
 *
 * @author losa
 */
public class DeletePeriodMapper extends Mapper<
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>,
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>> {
    @Override
    public void map(ProtobufWritable<ActivityAreaKey> key,
            ProtobufWritable<ActivityArea> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(ActivityAreaKey.class);
        final ActivityAreaKey oldActAreaKey = key.get();
        ProtobufWritable<ActivityAreaKey> newActAreaKey =
            ActivityAreaKeyUtil.createAndWrap(oldActAreaKey.getUserId(),
                                              0,
                                              oldActAreaKey.getIsWorkDay());
        context.write(newActAreaKey, value);
    }
}
