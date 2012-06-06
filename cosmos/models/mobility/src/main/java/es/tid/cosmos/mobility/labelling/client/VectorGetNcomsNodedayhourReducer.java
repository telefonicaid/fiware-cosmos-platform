package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <NodeBts, Null>
 * Output: <NodeBts, TwoInt>
 * 
 * @author dmicol
 */
public class VectorGetNcomsNodedayhourReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (ProtobufWritable<MobData> value : values) {
            valueCount++;
        }
        
        key.setConverter(NodeBts.class);
        final NodeBts bts = key.get();
        ProtobufWritable<NodeBts> outputBts = NodeBtsUtil.createAndWrap(
                bts.getUserId(), bts.getBts(), bts.getWeekday(), 0);
        TwoInt numComms = TwoIntUtil.create(bts.getRange(), valueCount);
        context.write(outputBts, MobDataUtil.createAndWrap(numComms));
    }
}
