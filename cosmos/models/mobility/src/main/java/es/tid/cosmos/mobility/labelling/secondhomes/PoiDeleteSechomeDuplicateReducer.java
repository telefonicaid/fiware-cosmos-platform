package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Null>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class PoiDeleteSechomeDuplicateReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Null>,
        ProtobufWritable<TwoInt>, MobilityWritable<Null>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        context.write(key, new MobilityWritable<Null>(Null.getDefaultInstance()));
    }
}
