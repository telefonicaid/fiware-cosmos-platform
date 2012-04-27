package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ClusterGetMajPoiByNodeReducer extends Reducer<LongWritable,
        ProtobufWritable<TwoInt>, LongWritable, LongWritable> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt maxPoiblCount = null;
        for (ProtobufWritable<TwoInt> value : values) {
            value.setConverter(TwoInt.class);
            final TwoInt poilblCount = value.get();
            if (maxPoiblCount == null ||
                    poilblCount.getNum2() > maxPoiblCount.getNum2()) {
                maxPoiblCount = poilblCount;
            }
        }
        context.write(key, new LongWritable(maxPoiblCount.getNum1()));
    }
}
