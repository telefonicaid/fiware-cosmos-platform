package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <Long, Text>
 * Output: <Long, Cdr>
 * 
 * @author dmicol, sortega
 */
public class ParseCdrMapper extends Mapper<LongWritable, Text, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final Cdr cdr = new CdrParser(value.toString()).parse();
            context.write(new LongWritable(cdr.getUserId()),
                          MobDataUtil.createAndWrap(cdr));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_CDRS).increment(1L);
        }
    }
}
