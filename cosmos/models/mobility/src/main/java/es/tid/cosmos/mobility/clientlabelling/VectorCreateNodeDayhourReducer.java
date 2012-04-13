package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.DailyVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorCreateNodeDayhourReducer extends Reducer
        <ProtobufWritable<NodeBts>, ProtobufWritable<TwoInt>,
        ProtobufWritable<TwoInt>, ProtobufWritable<DailyVector>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
                          Iterable<ProtobufWritable<TwoInt>> values,
                          Context context) throws IOException,
                                                  InterruptedException {
        final NodeBts bts = key.get();
        DailyVector.Builder vectorBuilder = DailyVector.newBuilder();
        long num2 = 0;
        for (int i = 0; i < 24; i++) {
            boolean added = false;
            for (ProtobufWritable<TwoInt> value : values) {
                num2 = value.get().getNum2();
                if (value.get().getNum1() == i) {
                    added = true;
                    break;
                }
            }
            if (!added) {
                num2 = 0;
            }
            TwoInt hourComms = TwoIntUtil.create(key.get().getWeekday(), num2);
            vectorBuilder.addHours(hourComms);
        }
        DailyVector vector = vectorBuilder.build();
        ProtobufWritable<DailyVector> vectorWrapper =
                ProtobufWritable.newInstance(DailyVector.class);
        vectorWrapper.set(vector);
        ProtobufWritable<TwoInt> node = TwoIntUtil.createAndWrap(
                bts.getUserId(), bts.getPlaceId());
        context.write(node, vectorWrapper);
    }
}
