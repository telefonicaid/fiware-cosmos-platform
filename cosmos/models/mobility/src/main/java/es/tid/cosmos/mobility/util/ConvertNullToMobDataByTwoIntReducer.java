package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ConvertNullToMobDataByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, NullWritable, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        for (NullWritable value : values) {
            context.write(key, MobDataUtil.createAndWrap(NullWritable.get()));
        }
    }
}
