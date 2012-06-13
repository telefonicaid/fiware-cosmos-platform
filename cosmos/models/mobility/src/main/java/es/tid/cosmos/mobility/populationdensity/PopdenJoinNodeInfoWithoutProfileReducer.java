package es.tid.cosmos.mobility.populationdensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.BtsProfileUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsProfile;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;

/**
 * Input: <Long, NodeBts>
 * Output: <BtsProfile, Int>
 *
 * @author ximo
 */
public class PopdenJoinNodeInfoWithoutProfileReducer extends Reducer<
        LongWritable, TypedProtobufWritable<NodeBts>, ProtobufWritable<BtsProfile>,
        TypedProtobufWritable<Int>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<NodeBts>> values, Context context)
            throws IOException, InterruptedException {
        for (TypedProtobufWritable<NodeBts> value : values) {
            final NodeBts nodeBts = value.get();
            context.write(BtsProfileUtil.createAndWrap(nodeBts.getBts(), 0,
                                  nodeBts.getWeekday(), nodeBts.getRange()),
                          TypedProtobufWritable.create(1));
        }
    }
}
