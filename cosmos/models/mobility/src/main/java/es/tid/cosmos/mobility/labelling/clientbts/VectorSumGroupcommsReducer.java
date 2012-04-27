package es.tid.cosmos.mobility.labelling.clientbts;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorSumGroupcommsReducer extends Reducer<
        ProtobufWritable<NodeBts>, IntWritable, ProtobufWritable<TwoInt>,
        ProtobufWritable<BtsCounter>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<IntWritable> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        ProtobufWritable<TwoInt> twoInt = TwoIntUtil.createAndWrap(
                nodeBts.getUserId(), nodeBts.getPlaceId());
        
        int ncoms = 0;
        for (IntWritable value : values) {
            ncoms += value.get();
        }
        ProtobufWritable<BtsCounter> btsCounter = BtsCounterUtil.createAndWrap(
                nodeBts.getPlaceId(), nodeBts.getWeekday(), nodeBts.getRange(),
                ncoms);
        context.write(twoInt, btsCounter);
    }
}
