package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class PoiFilterSechomeAdjacentReducer extends Reducer<
        ProtobufWritable<TwoInt>, LongWritable, ProtobufWritable<TwoInt>,
        NullWritable> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<LongWritable> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt pairbts = key.get();
        final String keyStr = TwoIntUtil.toString(pairbts);
        long nullValues = context.getCounter(Counters.NULL_VALUES.name(),
                                             keyStr).getValue();
        if (nullValues > 0) {
            return;
        }
        for (LongWritable value : values) {
            context.write(TwoIntUtil.createAndWrap(value.get(),
                    pairbts.getNum2()), NullWritable.get());
        }
    }
}
