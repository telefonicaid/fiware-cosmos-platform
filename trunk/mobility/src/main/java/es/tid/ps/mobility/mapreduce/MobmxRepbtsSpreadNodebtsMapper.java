package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.ps.mobility.data.MxProtocol.NodeBtsDay;
import es.tid.ps.mobility.data.MxProtocol.NodeMxCounter;
import es.tid.ps.mobility.data.NodeBtsDayUtil;

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
        final NodeMxCounter counter = value.get();
        for (int i = 0; i < counter.getBtsLength(); i++) {
            ProtobufWritable<NodeBtsDay> nodeWrapper = NodeBtsDayUtil.
                    createAndWrap(key.get(),counter.getBts(i).getBts(), 0, 0);
            context.write(nodeWrapper, new IntWritable(
                    counter.getBts(i).getCount()));
        }
    }
}
