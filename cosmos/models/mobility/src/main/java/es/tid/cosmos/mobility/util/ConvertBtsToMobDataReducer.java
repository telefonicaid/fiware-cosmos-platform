package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Bts;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 *
 * @author dmicol
 */
public class ConvertBtsToMobDataReducer extends Reducer<LongWritable,
        ProtobufWritable<Bts>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<Bts>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<Bts> value : values) {
            value.setConverter(Bts.class);
            final Bts bts = value.get();
            context.write(key, MobDataUtil.createAndWrap(bts));
        }
    }
}
