package es.tid.cosmos.mobility.btslabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class VectorFilterBtsReducer extends Reducer<LongWritable,
        ProtobufWritable<TwoInt>, ProtobufWritable<BtsCounter>, NullWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<TwoInt> value : values) {
            value.setConverter(TwoInt.class);
            final TwoInt groupHour = value.get();
            ProtobufWritable<BtsCounter> counter = BtsCounterUtil.createAndWrap(
                    key.get(), (int)groupHour.getNum1(),
                    (int)groupHour.getNum2(), 0);
            context.write(counter, NullWritable.get());
        }
    }
}
