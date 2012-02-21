package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.BtsCounterUtil;
import es.tid.ps.mobility.data.MxProtocol.BtsCounter;
import es.tid.ps.mobility.data.MxProtocol.NodeBts;
import es.tid.ps.mobility.data.MxProtocol.TwoInt;
import es.tid.ps.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class MobmxVectorSumGroupcommsReducer extends
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
                inNodeBts.getPhone(), inNodeBts.getBts());
        ProtobufWritable<BtsCounter> counterWrapper =
                BtsCounterUtil.createAndWrap(
                        inNodeBts.getBts(), inNodeBts.getWday(),
                        inNodeBts.getRange(), numberOfCommunications);
        context.write(nodeBtsWrapper, counterWrapper);
    }
}
