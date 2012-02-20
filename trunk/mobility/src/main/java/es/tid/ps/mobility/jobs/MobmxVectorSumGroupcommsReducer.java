package es.tid.ps.mobility.jobs;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import java.io.IOException;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.BtsCounterUtil;
import es.tid.ps.mobility.data.MxProtocol.BtsCounter;
import es.tid.ps.mobility.data.MxProtocol.NodeBts;
import es.tid.ps.mobility.data.MxProtocol.TwoInt;

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

        NodeBts inNodeBts = key.get();
        TwoInt nodeBts = TwoInt.newBuilder()
                .setNum1(inNodeBts.getPhone())
                .setNum2(inNodeBts.getBts())
                .build();
        ProtobufWritable<TwoInt> nodeBtsWrapper =
                ProtobufWritable.newInstance(TwoInt.class);
        nodeBtsWrapper.set(nodeBts);

        ProtobufWritable<BtsCounter> counterWrapper =
                BtsCounterUtil.createAndWrap(
                        inNodeBts.getBts(), inNodeBts.getWday(),
                        inNodeBts.getRange(), numberOfCommunications);

        context.write(nodeBtsWrapper, counterWrapper);
    }
}
