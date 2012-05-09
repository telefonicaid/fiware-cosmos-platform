package es.tid.cosmos.mobility.mivs;

import java.io.IOException;
import java.util.List;
import java.util.ArrayList;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.MobVars;
import es.tid.cosmos.mobility.data.MobViMobVarsUtil;

/**
 * Input: <Long, MobVars>
 * Output: <Long, MobViMobVars>
 * 
 * @author logc
 */
public class FusionTotalVarsReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<MobVars> allAreas = new ArrayList<MobVars>();
        for (ProtobufWritable<MobData> value: values) {
            value.setConverter(MobData.class);
            allAreas.add(value.get().getMobVars());
        }
        context.write(key, MobDataUtil.createAndWrap(
                MobViMobVarsUtil.create(allAreas)));
    }
}
