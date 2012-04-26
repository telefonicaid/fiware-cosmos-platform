package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjPutMaxIdReducer extends Reducer<LongWritable,
        ProtobufWritable<TwoInt>, LongWritable, ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt pairPois = null;
        long max = Long.MIN_VALUE;
        for (ProtobufWritable<TwoInt> value : values) {
            value.setConverter(TwoInt.class);
            pairPois = value.get();
            if (pairPois.getNum2() > max) {
                max = pairPois.getNum2();
            }
        }
        context.write(key, TwoIntUtil.createAndWrap(pairPois.getNum1(), max));
    }
}
