package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.DailyVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, DailyVector>
 * Output: <NodeBts, ClusterVector>
 *
 * @author dmicol
 */
public class VectorFuseNodeDaygroupReducer extends Reducer
        <ProtobufWritable<TwoInt>, TypedProtobufWritable<DailyVector>,
        ProtobufWritable<NodeBts>, TypedProtobufWritable<ClusterVector>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<DailyVector>> values, Context context)
            throws IOException, InterruptedException {
        List<DailyVector> valueList = new LinkedList<DailyVector>();
        for (TypedProtobufWritable<DailyVector> value : values) {
            final DailyVector dailyVector = value.get();
            valueList.add(dailyVector);
        }

        ClusterVector.Builder clusterVectorBuilder = ClusterVector.newBuilder();
        for (int group = 0; group < 4; group++) {
            boolean added = false;
            int j = 0;
            for (DailyVector dailyVector : valueList) {
                if (dailyVector.getHours(j).getNum1() == group) {
                    for (TwoInt hour : dailyVector.getHoursList()) {
                        clusterVectorBuilder.addComs(hour.getNum2());
                    }
                    added = true;
                    break;
                }
                j++;
            }
            if (!added) {
                for (int hour = 0; hour < 24; hour++) {
                    clusterVectorBuilder.addComs(0D);
                }
            }
        }

        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        ProtobufWritable<NodeBts> bts = NodeBtsUtil.createAndWrap(
                twoInt.getNum1(), (int)twoInt.getNum2(), 0, 0);
        context.write(bts,
                      new TypedProtobufWritable<ClusterVector>(
                              clusterVectorBuilder.build()));
    }
}
