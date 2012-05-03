package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Long>
 * 
 * @author dmicol
 */
public class AdjSpreadCountReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (ProtobufWritable<MobData> value : values) {
            valueCount++;
        }
        context.write(new LongWritable(0L),
                      MobDataUtil.createAndWrap(valueCount));
    }
}
