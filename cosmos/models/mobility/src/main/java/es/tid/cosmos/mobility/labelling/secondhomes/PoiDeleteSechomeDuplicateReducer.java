package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Null>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class PoiDeleteSechomeDuplicateReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        context.write(key, new TypedProtobufWritable<Null>(Null.getDefaultInstance()));
    }
}
