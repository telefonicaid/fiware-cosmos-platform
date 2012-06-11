package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cdr;

/**
 * Input: <Long, Text>
 * Output: <Long, Cdr>
 * 
 * @author sortega
 */
public class ParseCdrsReducer extends Reducer<LongWritable, Text, LongWritable,
        MobilityWritable<Cdr>> {
    private LongWritable userId;
    
    @Override
    public void setup(Context context) {
        this.userId = new LongWritable();
    }
    
    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            Cdr cdr;
            try {
                cdr = new CdrParser(value.toString()).parse();
            } catch (Exception ex) {
                context.getCounter(Counters.INVALID_LINES).increment(1L);
                continue;
            }
            this.userId.set(cdr.getUserId());
            context.write(this.userId, new MobilityWritable<Cdr>(cdr));
        }
    }
}
