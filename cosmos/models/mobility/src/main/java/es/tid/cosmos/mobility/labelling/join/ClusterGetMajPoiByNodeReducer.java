package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Long>
 *
 * @author dmicol
 */
class ClusterGetMajPoiByNodeReducer extends Reducer<LongWritable,
        TypedProtobufWritable<TwoInt>, LongWritable, TypedProtobufWritable<Int64>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt maxPoiblCount = null;
        for (TypedProtobufWritable<TwoInt> value : values) {
            final TwoInt poilblCount = value.get();
            if (maxPoiblCount == null ||
                    poilblCount.getNum2() > maxPoiblCount.getNum2()) {
                maxPoiblCount = poilblCount;
            }
        }
        context.write(key, TypedProtobufWritable.create(maxPoiblCount.getNum1()));
    }
}
