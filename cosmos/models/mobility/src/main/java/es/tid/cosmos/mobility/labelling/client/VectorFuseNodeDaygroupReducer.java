package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.DailyVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.NodeBtsUtil;

/**
 *
 * @author dmicol
 */
public class VectorFuseNodeDaygroupReducer extends Reducer
        <ProtobufWritable<TwoInt>, ProtobufWritable<DailyVector>,
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
                          Iterable<ProtobufWritable<DailyVector>> values,
                          Context context) throws IOException,
                                                  InterruptedException {
        List<DailyVector> valueList = new LinkedList<DailyVector>();
        for (ProtobufWritable<DailyVector> value : values) {
            value.setConverter(DailyVector.class);
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
                for (int k = 0; k < 24; k++) {
                    clusterVectorBuilder.addComs(0D);
                }
            }
        }

        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        ProtobufWritable<NodeBts> bts = NodeBtsUtil.createAndWrap(
                twoInt.getNum1(), (int)twoInt.getNum2(), 0, 0);
        ProtobufWritable<ClusterVector> clusterVector =
                ProtobufWritable.newInstance(ClusterVector.class);
        clusterVector.set(clusterVectorBuilder.build());
        context.write(bts, clusterVector);
    }
}
