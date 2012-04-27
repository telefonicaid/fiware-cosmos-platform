package es.tid.cosmos.mobility.labelling.bts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.NodeBtsUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorSumComsBtsReducer extends Reducer<
        ProtobufWritable<BtsCounter>, NullWritable, ProtobufWritable<NodeBts>,
        ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<BtsCounter> key,
            Iterable<NullWritable> values, Context context)
            throws IOException,InterruptedException {
        int valueCount = 0;
        for (NullWritable value : values) {
            valueCount++;
        }
        
        key.setConverter(BtsCounter.class);
        final BtsCounter counter = key.get();
        ProtobufWritable<NodeBts> nodeBts = NodeBtsUtil.createAndWrap(
                0L, (int)counter.getPlaceId(), counter.getWeekday(), 0);
        ProtobufWritable<TwoInt> hourComms = TwoIntUtil.createAndWrap(
                counter.getRange(), valueCount);
        context.write(nodeBts, hourComms);
    }
}
