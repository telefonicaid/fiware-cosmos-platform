package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class PoiDeleteSechomeDuplicateReducer extends Reducer<
        ProtobufWritable<TwoInt>, NullWritable, ProtobufWritable<TwoInt>,
        NullWritable> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        context.write(key, NullWritable.get());
    }
}
