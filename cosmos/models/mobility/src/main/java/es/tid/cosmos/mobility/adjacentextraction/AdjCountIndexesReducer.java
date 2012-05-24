package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;

/**
 * Input: <LongWritable, LongWritable>
 * Output: <LongWritable, NullWritable>
 *
 * @author dmicol
 */
public class AdjCountIndexesReducer extends Reducer <LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        long sum = 0L;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            sum += value.get().getLong();
        }
        context.getCounter(Counters.NUM_INDEXES).increment(sum);
        context.write(new LongWritable(sum),
                      MobDataUtil.createAndWrap(NullWritable.get()));
    }
}
