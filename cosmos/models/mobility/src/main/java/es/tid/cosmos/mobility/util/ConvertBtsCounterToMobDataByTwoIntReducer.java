package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 *
 * @author dmicol
 */
public class ConvertBtsCounterToMobDataByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<BtsCounter>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<BtsCounter>> values, Context context)
            throws IOException, InterruptedException {
        for (ProtobufWritable<BtsCounter> value : values) {
            value.setConverter(BtsCounter.class);
            final BtsCounter btsCounter = value.get();
            context.write(key, MobDataUtil.createAndWrap(btsCounter));
        }
    }
}
