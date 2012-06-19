package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClientProfile;

/**
 *
 * Input: <Long, Text>
 * Output: <Long, ClientProfile>
 * 
 * @author dmicol, ximo
 */
public class ParseClientProfileMapper extends Mapper<LongWritable, Text,
        LongWritable, TypedProtobufWritable<ClientProfile>> {
    private String separator;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.separator = context.getConfiguration().get(Config.DATA_SEPARATOR);
    }
    
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final ClientProfile clientProfile = new ClientProfileParser(
                    value.toString(), this.separator).parse();
            context.write(new LongWritable(clientProfile.getUserId()),
                          new TypedProtobufWritable<ClientProfile>(clientProfile));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_CLIENT_PROFILES).increment(1L);
        }
    }
}