package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;
import es.tid.cosmos.mobility.data.MobProtocol.Cell;

/**
 *
 * @author losa
 */
public class ActivityAreaMapper extends Mapper<
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<Cell>,
            ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>> {
    @Override
    public void map(ProtobufWritable<ActivityAreaKey> key,
                    ProtobufWritable<Cell> value, Context context)
            throws IOException, InterruptedException {
        final Cell cell = value.get();
        int numPos = 1;
        int difBtss = 1;
        int difMuns = 1;
        int difStates = 1;
        double masscenterUtmX = cell.getPosx();
        double masscenterUtmY = cell.getPosy();
        double radius = 0.0;
        double diamAreaInf = 0.0;

        ActivityArea activityArea =
            ActivityAreaUtil.create(numPos, difBtss, difMuns, difStates,
                                    masscenterUtmX, masscenterUtmY,
                                    radius, diamAreaInf);
        ProtobufWritable<ActivityArea> ans = ActivityAreaUtil.wrap(activityArea);
        context.write(key, ans);
    }
}
