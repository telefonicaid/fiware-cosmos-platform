package es.tid.cosmos.mobility.labelling.join;

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
public class ClusterCountMajPoiByNodeReducer extends Reducer<
        ProtobufWritable<TwoInt>, NullWritable, LongWritable,
        ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (NullWritable value : values) {
            valueCount++;
        }
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        context.write(new LongWritable(twoInt.getNum1()),
                      TwoIntUtil.createAndWrap(twoInt.getNum2(), valueCount));
    }
}
