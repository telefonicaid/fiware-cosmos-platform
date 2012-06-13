package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 * Input: <NodeBtsDay, Int>
 * Output: <Long, NodeBtsDay>
 *
 * @author dmicol, sortega
 */
public class RepbtsAggbybtsReducer extends Reducer<ProtobufWritable<NodeBtsDay>,
        TypedProtobufWritable<Int>, LongWritable, TypedProtobufWritable<NodeBtsDay>> {
    @Override
    public void reduce(ProtobufWritable<NodeBtsDay> key,
            Iterable<TypedProtobufWritable<Int>> values, Context context)
            throws IOException, InterruptedException {
        int totalCallCount = 0;
        for (TypedProtobufWritable<Int> value : values) {
            totalCallCount += value.get().getValue();
        }
        key.setConverter(NodeBtsDay.class);
        final NodeBtsDay byDay = key.get();
        NodeBtsDay nodeBtsDay = NodeBtsDayUtil.create(byDay.getUserId(),
                byDay.getBts(), byDay.getWorkday(), totalCallCount);
        context.write(new LongWritable(byDay.getUserId()),
                      new TypedProtobufWritable<NodeBtsDay>(nodeBtsDay));
    }
}
