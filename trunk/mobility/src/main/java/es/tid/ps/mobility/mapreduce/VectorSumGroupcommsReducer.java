package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.BtsCounterUtil;
import es.tid.ps.mobility.data.MobProtocol.BtsCounter;
import es.tid.ps.mobility.data.MobProtocol.NodeBts;
import es.tid.ps.mobility.data.MobProtocol.TwoInt;
import es.tid.ps.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class VectorSumGroupcommsReducer extends
        Reducer<ProtobufWritable<NodeBts>, IntWritable,
        ProtobufWritable<TwoInt>, ProtobufWritable<BtsCounter>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<IntWritable> values, Context context) throws IOException,
            InterruptedException {
        int numberOfCommunications = 0;
        for (IntWritable value : values) {
            numberOfCommunications += value.get();
        }

        final NodeBts inNodeBts = key.get();
        ProtobufWritable<TwoInt> nodeBtsWrapper = TwoIntUtil.createAndWrap(
                inNodeBts.getUserId(), inNodeBts.getPlaceId());
        ProtobufWritable<BtsCounter> counterWrapper =
                BtsCounterUtil.createAndWrap(
                        inNodeBts.getPlaceId(), inNodeBts.getWeekday(),
                        inNodeBts.getRange(), numberOfCommunications);
        context.write(nodeBtsWrapper, counterWrapper);
    }
}
