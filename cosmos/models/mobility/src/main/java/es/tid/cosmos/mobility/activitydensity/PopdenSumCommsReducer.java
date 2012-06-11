package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <BtsProfile, Int>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
public class PopdenSumCommsReducer extends Reducer<ProtobufWritable<BtsProfile>,
        MobilityWritable<Int>, ProtobufWritable<TwoInt>,
        MobilityWritable<BtsCounter>> {
    @Override
    protected void reduce(ProtobufWritable<BtsProfile> key,
            Iterable<MobilityWritable<Int>> values, Context context)
            throws IOException, InterruptedException {
        int nComsTotal = 0;
        for (MobilityWritable<Int> value : values) {
            nComsTotal += value.get().getNum();
        }
        key.setConverter(BtsProfile.class);
        final BtsProfile btsWdayhour = key.get();
        ProtobufWritable<TwoInt> btsProfile =
                TwoIntUtil.createAndWrap(btsWdayhour.getBts(),
                                         btsWdayhour.getProfile());
        MobilityWritable<BtsCounter> counter = new MobilityWritable<BtsCounter>(
                BtsCounterUtil.create(0L, btsWdayhour.getWday(),
                                      btsWdayhour.getHour(), nComsTotal));
        context.write(btsProfile, counter);
    }
}
