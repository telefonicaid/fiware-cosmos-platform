package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsUtil;
import es.tid.cosmos.mobility.data.MobProtocol.Bts;

/**
 *
 * @author dmicol
 */
public class BorrarGetBtsComareaReducer extends Reducer<LongWritable, Text,
        LongWritable, ProtobufWritable<Bts>> {
    @Override
    protected void reduce(LongWritable key, Iterable<Text> values,
            Context context) throws IOException, InterruptedException {
        for (Text value : values) {
            Bts bts = new BtsParser(value.toString()).parse();
            context.write(new LongWritable(bts.getPlaceId()),
                          BtsUtil.wrap(bts));
        }
    }
}
