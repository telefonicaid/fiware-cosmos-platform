package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Bts;

/**
 * Input: <Long, Text>
 * Output: <Long, Bts>
 * 
 * @author dmicol
 */
public class BorrarGetBtsComareaReducer extends Reducer<LongWritable, Text,
        LongWritable, MobilityWritable<Bts>> {
    @Override
    protected void reduce(LongWritable key, Iterable<Text> values,
            Context context) throws IOException, InterruptedException {
        for (Text value : values) {
            Bts bts;
            try {
                bts = new BtsParser(value.toString()).parse();
            } catch (Exception ex) {
                context.getCounter(Counters.INVALID_LINES).increment(1L);
                continue;
            }
            context.write(new LongWritable(bts.getPlaceId()),
                          new MobilityWritable<Bts>(bts, Bts.class));
        }
    }
}
