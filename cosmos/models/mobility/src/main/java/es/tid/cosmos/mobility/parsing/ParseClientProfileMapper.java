package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.util.Logger;
import es.tid.cosmos.mobility.conf.MobilityConfiguration;
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
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }
    
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final String line = value.toString();
        try {
            final ClientProfile clientProfile = new ClientProfileParser(
                    line, this.separator).parse();
            context.write(new LongWritable(clientProfile.getUserId()),
                          new TypedProtobufWritable<ClientProfile>(clientProfile));
            context.getCounter(Counters.VALID_RECORDS).increment(1L);
        } catch (Exception ex) {
            Logger.get(ParseClientProfileMapper.class).warn("Invalid line: "
                                                            + line);
            context.getCounter(Counters.INVALID_RECORDS).increment(1L);
        }
    }
}