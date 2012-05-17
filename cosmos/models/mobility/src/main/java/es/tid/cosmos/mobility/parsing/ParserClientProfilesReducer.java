package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import com.twitter.elephantbird.util.Pair;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, Text>
 * Output: <Long, Int>
 * 
 * @author dmicol
 */
public class ParserClientProfilesReducer extends Reducer<LongWritable, Text,
        LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key, Iterable<Text> values,
            Context context) throws IOException, InterruptedException {
        for (Text value : values) {
            Pair<Long, Integer> clientProfile;
            try {
                clientProfile = new ClientProfileParser(
                        value.toString()).parse();
            } catch (Exception ex) {
                context.getCounter(Counters.INVALID_LINES).increment(1L);
                continue;
            }
            context.write(new LongWritable(clientProfile.getFirst()),
                          MobDataUtil.createAndWrap(clientProfile.getSecond()));
        }
    }
}
