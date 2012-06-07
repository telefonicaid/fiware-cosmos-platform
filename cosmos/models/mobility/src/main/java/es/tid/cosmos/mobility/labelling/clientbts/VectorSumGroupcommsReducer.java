package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <NodeBts, Int>
 * Output: <TwoInt, BtsCounter>
 * 
 * @author dmicol
 */
public class VectorSumGroupcommsReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        ProtobufWritable<TwoInt> twoInt = TwoIntUtil.createAndWrap(
                nodeBts.getUserId(), nodeBts.getBts());
        
        int ncoms = 0;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            ncoms += value.get().getInt();
        }
        BtsCounter btsCounter = BtsCounterUtil.create(nodeBts.getBts(),
                nodeBts.getWeekday(), nodeBts.getRange(), ncoms);
        context.write(twoInt, MobDataUtil.createAndWrap(btsCounter));
    }
}
