package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ConvertLongToMobDataByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, LongWritable, ProtobufWritable<TwoInt>,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<LongWritable> values, Context context)
            throws IOException, InterruptedException {
        for (LongWritable value : values) {
            context.write(key, MobDataUtil.createAndWrap(value.get()));
        }
    }
}
