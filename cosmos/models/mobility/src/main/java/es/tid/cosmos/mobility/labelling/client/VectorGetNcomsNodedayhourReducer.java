package es.tid.cosmos.mobility.labelling.client;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorGetNcomsNodedayhourReducer extends Reducer<
        ProtobufWritable<NodeBts>, NullWritable, ProtobufWritable<NodeBts>,
        ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
                          Iterable<NullWritable> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (NullWritable unused : values) {
            valueCount++;
        }
        
        key.setConverter(NodeBts.class);
        final NodeBts bts = key.get();
        ProtobufWritable<NodeBts> outputBts = NodeBtsUtil.createAndWrap(
                bts.getUserId(), bts.getPlaceId(), bts.getWeekday(), 0);
        ProtobufWritable<TwoInt> numComms = TwoIntUtil.createAndWrap(
                bts.getRange(), valueCount);
        context.write(outputBts, numComms);
    }
}
