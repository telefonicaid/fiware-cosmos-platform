package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.MobViMobVars;

/**
 *
 * @author dmicol
 */
public class ConvertMobViMobVarsToMobDataReducer extends Reducer<LongWritable,
        ProtobufWritable<MobViMobVars>, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobViMobVars>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<MobViMobVars> value : values) {
            value.setConverter(MobViMobVars.class);
            final MobViMobVars mobViMobVars = value.get();
            context.write(key, MobDataUtil.createAndWrap(mobViMobVars));
        }
    }
}
