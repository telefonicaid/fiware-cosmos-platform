package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.ActivityAreaUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityArea;
import es.tid.cosmos.mobility.data.MobProtocol.ActivityAreaKey;

/**
 *
 * @author losa
 */
public class ActivityAreaReducer extends Reducer<
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>,
        ProtobufWritable<ActivityAreaKey>, ProtobufWritable<ActivityArea>> {
    @Override
    protected void reduce(ProtobufWritable<ActivityAreaKey> key,
            Iterable<ProtobufWritable<ActivityArea>> values, Context context)
            throws IOException, InterruptedException {
        int numPos = 0;
        int difBtss = 0;
        int difMuns = 0;
        int difStates = 0;
        double masscenterUtmX = 0.0;
        double masscenterUtmY = 0.0;
        double radius = 0.0;
        double diamAreaInf = 0.0;

        for (ProtobufWritable<ActivityArea> value : values) {
            ActivityArea tmp = value.get();
            numPos += 1;
            difBtss += 1;
            difMuns += 1;
            difStates += 1;
            masscenterUtmX = tmp.getMasscenterUtmX();
            masscenterUtmY = tmp.getMasscenterUtmY();
        }
        ProtobufWritable<ActivityArea> ans =
                ActivityAreaUtil.createAndWrapArea(numPos, difBtss, difMuns,
                                                   difStates, masscenterUtmX,
                                                   masscenterUtmY, radius,
                                                   diamAreaInf);
        context.write(key, ans);
    }
}
