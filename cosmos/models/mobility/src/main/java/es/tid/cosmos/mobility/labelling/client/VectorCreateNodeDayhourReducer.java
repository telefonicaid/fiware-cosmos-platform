package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

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
        key.setConverter(NodeBts.class);
        final NodeBts bts = key.get();
        
        List<TwoInt> valueList = new LinkedList<TwoInt>();
        for (ProtobufWritable<TwoInt> value : values) {
            value.setConverter(TwoInt.class);
            final TwoInt hourComms = value.get();
            valueList.add(hourComms);
        }
        
        DailyVector.Builder vectorBuilder = DailyVector.newBuilder();
        for (int i = 0; i < 24; i++) {
            long num2 = 0L;
            boolean added = false;
            for (TwoInt hourComms : valueList) {
                num2 = hourComms.getNum2();
                if (hourComms.getNum1() == i) {
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
                bts.getUserId(), bts.getPlaceId());
        ProtobufWritable<DailyVector> vector =
                ProtobufWritable.newInstance(DailyVector.class);
        vector.set(vectorBuilder.build());
        context.write(node, vector);
    }
}
