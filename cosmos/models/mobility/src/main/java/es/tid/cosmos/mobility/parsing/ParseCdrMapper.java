package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 * Input: <Long, Text>
 * Output: <Long, Cdr>
 * 
 * @author dmicol, sortega
 */
public class ParseCdrMapper extends Mapper<LongWritable, Text, LongWritable,
        TypedProtobufWritable<Cdr>> {
    private String separator;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(
                context.getConfiguration());
        this.separator = conf.getDataSeparator();
    }
    
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final Cdr cdr = new CdrParser(value.toString(),
                                          this.separator).parse();
            context.write(new LongWritable(cdr.getUserId()),
                          new TypedProtobufWritable<Cdr>(cdr));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_CDRS).increment(1L);
        }
    }
}
