package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.DailyVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <NodeBts, TwoInt>
 * Output: <TwoInt, DailyVector>
 *
 * @author dmicol
 */
public class VectorCreateNodeDayhourReducer extends Reducer
        <ProtobufWritable<NodeBts>, TypedProtobufWritable<TwoInt>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<DailyVector>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<TypedProtobufWritable<TwoInt>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts bts = key.get();

        List<TwoInt> valueList = new LinkedList<TwoInt>();
        for (TypedProtobufWritable<TwoInt> value : values) {
            valueList.add(value.get());
        }

        DailyVector.Builder vectorBuilder = DailyVector.newBuilder();
        for (int hour = 0; hour < 24; hour++) {
            long num2 = 0L;
            boolean added = false;
            for (TwoInt hourComms : valueList) {
                num2 = hourComms.getNum2();
                if (hourComms.getNum1() == hour) {
                    added = true;
                    break;
                }
            }
            if (!added) {
                num2 = 0L;
            }
            TwoInt hourComms = TwoIntUtil.create(bts.getWeekday(), num2);
            vectorBuilder.addHours(hourComms);
        }
        ProtobufWritable<TwoInt> node = TwoIntUtil.createAndWrap(
                bts.getUserId(), bts.getBts());
        context.write(node, new TypedProtobufWritable<DailyVector>(
                vectorBuilder.build()));
    }
}
