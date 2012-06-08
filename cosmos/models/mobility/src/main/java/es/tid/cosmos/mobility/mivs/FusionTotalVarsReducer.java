package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobViMobVarsUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobViMobVars;

/**
 * Input: <Long, MobVars>
 * Output: <Long, MobViMobVars>
 * 
 * @author logc
 */
public class FusionTotalVarsReducer extends Reducer<LongWritable,
        MobilityWritable<MobVars>, LongWritable, MobilityWritable<MobViMobVars>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<MobilityWritable<MobVars>> values, Context context)
            throws IOException, InterruptedException {
        List<MobVars> allAreas = new ArrayList<MobVars>();
        for (MobilityWritable<MobVars> value: values) {
            allAreas.add(value.get());
        }
        context.write(key, new MobilityWritable<MobViMobVars>(
                MobViMobVarsUtil.create(allAreas),
                MobViMobVars.class));
    }
}
