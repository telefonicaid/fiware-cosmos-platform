package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * Input: <Long, Text>
 * Output: <Long, ClientProfile>
 * 
 * @author ximo
 */
public class ParseClientProfilesReducer extends Reducer<LongWritable, Text,
        LongWritable, ProtobufWritable<MobData>> {
    private LongWritable userId;
    
    @Override
    public void setup(Reducer.Context context) {
        this.userId = new LongWritable();
    }
    
    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            ClientProfile clientProfile;
            try {
                clientProfile = new ClientProfileParser(value.toString()).parse();
            } catch (Exception ex) {
                context.getCounter(Counters.INVALID_LINES).increment(1L);
                continue;
            }
            this.userId.set(clientProfile.getUserId());
            context.write(this.userId, MobDataUtil.createAndWrap(clientProfile));
        }
    }
}