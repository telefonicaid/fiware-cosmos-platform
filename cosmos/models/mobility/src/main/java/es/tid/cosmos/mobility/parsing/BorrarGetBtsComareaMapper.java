package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 * Input: <Long, Text>
 * Output: <Long, Bts>
 * 
 * @author dmicol
 */
public class BorrarGetBtsComareaMapper extends Mapper<LongWritable, Text,
        LongWritable, MobilityWritable<Bts>> {
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        try {
            final Bts bts = new BtsParser(value.toString()).parse();
            context.write(new LongWritable(bts.getPlaceId()),
                          new MobilityWritable<Bts>(bts));
        } catch (Exception ex) {
            context.getCounter(Counters.INVALID_BTS).increment(1L);
        }
    }
}
