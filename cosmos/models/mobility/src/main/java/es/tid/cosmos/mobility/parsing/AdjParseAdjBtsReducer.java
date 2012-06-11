package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, Text>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class AdjParseAdjBtsReducer extends Reducer<LongWritable, Text,
        ProtobufWritable<TwoInt>, MobilityWritable<Null>> {
    @Override
    protected void reduce(LongWritable key, Iterable<Text> values,
            Context context) throws IOException, InterruptedException {
        for (Text value : values) {
            TwoInt adjBts;
            try {
                adjBts = new AdjacentParser(value.toString()).parse();
            } catch (Exception ex) {
                context.getCounter(Counters.INVALID_LINES).increment(1L);
                continue;
            }
            context.write(TwoIntUtil.wrap(adjBts),
                          new MobilityWritable(Null.getDefaultInstance()));
        }
    }
}
