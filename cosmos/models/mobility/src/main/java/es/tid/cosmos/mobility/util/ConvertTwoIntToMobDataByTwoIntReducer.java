package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ConvertTwoIntToMobDataByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<TwoInt>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<TwoInt> value : values) {
            value.setConverter(TwoInt.class);
            final TwoInt twoInt = value.get();
            context.write(key, MobDataUtil.createAndWrap(twoInt));
        }
    }
}
