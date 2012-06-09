package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 *
 * Input: <Long, Text>
 * Output: <Long, ClientProfile>
 * 
 * @author dmicol, ximo
 */
public class ParseClientProfileMapper extends Mapper<LongWritable, Text,
        LongWritable, ProtobufWritable<MobData>> {
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final ClientProfile clientProfile = new ClientProfileParser(
                    value.toString()).parse();
            context.write(new LongWritable(clientProfile.getUserId()),
                          MobDataUtil.createAndWrap(clientProfile));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_LINES).increment(1L);
        }
    }
}