package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <BtsCounter, Null>
 * Output: <NodeBts, TwoInt>
 * 
 * @author dmicol
 */
public class VectorSumComsBtsReducer extends Reducer<
        ProtobufWritable<BtsCounter>, ProtobufWritable<MobData>,
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<BtsCounter> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException,InterruptedException {
        int valueCount = 0;
        for (ProtobufWritable<MobData> value : values) {
            valueCount++;
        }
        
        key.setConverter(BtsCounter.class);
        final BtsCounter counter = key.get();
        ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                0L, counter.getBts(), counter.getWeekday(), 0);
        TwoInt hourComms = TwoIntUtil.create(counter.getRange(), valueCount);
        context.write(nodeBts, MobDataUtil.createAndWrap(hourComms));
    }
}
