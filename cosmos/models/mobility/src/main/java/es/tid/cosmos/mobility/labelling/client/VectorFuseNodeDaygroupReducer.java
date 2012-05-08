package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.DailyVector;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.NodeBtsUtil;

/**
 * Input: <TwoInt, DailyVector>
 * Output: <NodeBts, ClusterVector>
 * 
 * @author dmicol
 */
public class VectorFuseNodeDaygroupReducer extends Reducer
        <ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<DailyVector> valueList = new LinkedList<DailyVector>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final DailyVector dailyVector = value.get().getDailyVector();
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
                      MobDataUtil.createAndWrap(clusterVectorBuilder.build()));
    }
}
