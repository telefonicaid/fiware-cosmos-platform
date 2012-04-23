package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class SetMobDataInputIdByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    private static final int DEFAULT_INVALID_ID = -1;
    private static Integer inputId = null;

    @Override
    protected void setup(Context context) throws IOException,
            InterruptedException {
        final Configuration conf = context.getConfiguration();
        inputId = conf.getInt("input_id", DEFAULT_INVALID_ID);
        if (inputId == DEFAULT_INVALID_ID) {
            throw new IllegalArgumentException("Missing or invalid input ID");
        }
    }
    
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            MobData mobData = value.get();
            MobData mobDataWithInputId = MobDataUtil.setInputId(mobData,
                                                                inputId);
            context.write(key, MobDataUtil.wrap(mobDataWithInputId));
        }
    }
}
