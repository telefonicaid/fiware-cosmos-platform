package es.tid.cosmos.mobility.clientlabelling;

import java.io.IOException;

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
        ClusterVector.Builder clusterVectorBuilder = ClusterVector.newBuilder();
        for (int group = 0; group < 4; group++) {
            boolean added = false;
            int j = 0;
            for (ProtobufWritable<DailyVector> value : values) {
                final DailyVector dailyVector = value.get();
                if (dailyVector.getHours(j).getNum2() == group) {
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
                    clusterVectorBuilder.addComs(0);
                }
            }
        }

        ProtobufWritable<NodeBts> bts = NodeBtsUtil.createAndWrap(
                key.get().getNum1(), (int)key.get().getNum2(), 0, 0);
        ProtobufWritable<ClusterVector> clusterVectorWrapper =
                ProtobufWritable.newInstance(ClusterVector.class);
        clusterVectorWrapper.set(clusterVectorBuilder.build());
        context.write(bts, clusterVectorWrapper);
    }
}
