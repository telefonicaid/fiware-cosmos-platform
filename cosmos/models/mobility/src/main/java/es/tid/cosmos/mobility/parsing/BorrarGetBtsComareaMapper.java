package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 * Input: <Long, Text>
 * Output: <Long, Bts>
 * 
 * @author dmicol
 */
class BorrarGetBtsComareaMapper extends Mapper<LongWritable, Text,
        LongWritable, TypedProtobufWritable<Bts>> {
    private String separator;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }
    
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final Bts bts = new BtsParser(value.toString(),
                                          this.separator).parse();
            context.write(new LongWritable(bts.getPlaceId()),
                          new TypedProtobufWritable<Bts>(bts));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_BTS).increment(1L);
        }
    }
}
