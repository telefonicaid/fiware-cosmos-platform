package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <NodeBts, Null>
 * Output: <NodeBts, TwoInt>
 * 
 * @author dmicol
 */
public class VectorGetNcomsNodedayhourReducer extends Reducer<
        ProtobufWritable<NodeBts>, TypedProtobufWritable<Null>,
        ProtobufWritable<NodeBts>, TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<TypedProtobufWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (TypedProtobufWritable<Null> value : values) {
            valueCount++;
        }
        
        key.setConverter(NodeBts.class);
        final NodeBts bts = key.get();
        ProtobufWritable<NodeBts> outputBts = NodeBtsUtil.createAndWrap(
                bts.getUserId(), bts.getBts(), bts.getWeekday(), 0);
        TwoInt numComms = TwoIntUtil.create(bts.getRange(), valueCount);
        context.write(outputBts, new TypedProtobufWritable<TwoInt>(numComms));
    }
}
