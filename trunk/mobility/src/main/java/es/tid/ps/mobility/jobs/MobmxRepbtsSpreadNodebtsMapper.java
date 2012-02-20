package es.tid.ps.mobility.jobs;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.MxProtocol.NodeBtsDay;
import es.tid.ps.mobility.data.MxProtocol.NodeMxCounter;

/**
 *
 * @author dmicol
 */
public class MobmxRepbtsSpreadNodebtsMapper extends Mapper<IntWritable,
        ProtobufWritable<NodeMxCounter>, ProtobufWritable<NodeBtsDay>,
        IntWritable> {
    @Override
    public void map(IntWritable key, ProtobufWritable<NodeMxCounter> value,
            Context context) throws IOException, InterruptedException {
        NodeMxCounter counter = value.get();
        for (int i = 0; i < counter.getBtsLength(); i++) {
            NodeBtsDay node = NodeBtsDay.newBuilder()
                    .setNode(key.get())
                    .setBts(counter.getBts(i).getBts())
                    .setWorkday(0)
                    .setCount(0)
                    .build();
            ProtobufWritable<NodeBtsDay> nodeWrapper =
                    ProtobufWritable.newInstance(NodeBtsDay.class);
            nodeWrapper.set(node);
            context.write(nodeWrapper, new IntWritable(
                    counter.getBts(i).getCount()));
        }
    }
}
