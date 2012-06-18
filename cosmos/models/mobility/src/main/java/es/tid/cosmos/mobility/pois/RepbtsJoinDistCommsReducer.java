package es.tid.cosmos.mobility.pois;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.google.protobuf.Message;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Reducer.Context;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int;
import es.tid.cosmos.mobility.data.BtsCounterUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;

/**
 * Input: <Long, Int|NodeBtsDay>
 * Output: <Long, BtsCounter>
 * 
 * @author dmicol
 */
class RepbtsJoinDistCommsReducer extends Reducer<LongWritable,
        TypedProtobufWritable<Message>, LongWritable, TypedProtobufWritable<BtsCounter>> {
    @Override
    public void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        List<Integer> ncommsList = new LinkedList<Integer>();
        List<NodeBtsDay> nodeBtsDayList = new LinkedList<NodeBtsDay>();
        for (TypedProtobufWritable<Message> value : values) {
            final Message message = value.get();
            if (message instanceof Int) {
                ncommsList.add(((Int) message).getValue());
            } else if (message instanceof NodeBtsDay) {
                nodeBtsDayList.add((NodeBtsDay) message);
            } else {
                throw new IllegalArgumentException("Unexpected input type: "
                        + message.getClass());
            }
        }
        for (Integer ncomms : ncommsList) {
            for (NodeBtsDay nodeBtsDay : nodeBtsDayList) {
                BtsCounter counter = BtsCounterUtil.create(
                                nodeBtsDay.getBts(),
                                0,
                                nodeBtsDay.getCount(),
                                nodeBtsDay.getCount() * 100 / ncomms);
                context.write(key, new TypedProtobufWritable<BtsCounter>(counter));
            }
        }
    }
}
