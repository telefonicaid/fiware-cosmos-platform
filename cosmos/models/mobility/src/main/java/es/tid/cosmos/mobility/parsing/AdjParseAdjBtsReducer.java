package es.tid.cosmos.mobility.parsing;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <Long, Text>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class AdjParseAdjBtsReducer extends Reducer<LongWritable, Text,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
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
                          MobDataUtil.createAndWrap(NullWritable.get()));
        }
    }
}
