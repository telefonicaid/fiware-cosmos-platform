package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ConvertIntToMobDataReducer extends Reducer<LongWritable,
        IntWritable, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key, Iterable<IntWritable> values,
            Context context) throws IOException, InterruptedException {
        for (IntWritable value : values) {
            context.write(key, MobDataUtil.createAndWrap(value.get()));
        }
    }
}
