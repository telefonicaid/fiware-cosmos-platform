package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <BtsProfile, Int>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
public class PopdenSumCommsReducer extends Reducer<ProtobufWritable<BtsProfile>,
        TypedProtobufWritable<Int>, ProtobufWritable<TwoInt>,
        TypedProtobufWritable<BtsCounter>> {
    @Override
    protected void reduce(ProtobufWritable<BtsProfile> key,
            Iterable<TypedProtobufWritable<Int>> values, Context context)
            throws IOException, InterruptedException {
        int nComsTotal = 0;
        for (TypedProtobufWritable<Int> value : values) {
            nComsTotal += value.get().getValue();
        }
        key.setConverter(BtsProfile.class);
        final BtsProfile btsWdayhour = key.get();
        ProtobufWritable<TwoInt> btsProfile =
                TwoIntUtil.createAndWrap(btsWdayhour.getBts(),
                                         btsWdayhour.getProfile());
        TypedProtobufWritable<BtsCounter> counter = new TypedProtobufWritable<BtsCounter>(
                BtsCounterUtil.create(0L, btsWdayhour.getWday(),
                                      btsWdayhour.getHour(), nComsTotal));
        context.write(btsProfile, counter);
    }
}
