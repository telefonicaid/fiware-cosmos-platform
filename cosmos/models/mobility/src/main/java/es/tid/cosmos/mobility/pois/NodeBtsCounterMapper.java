package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.NodeBtsUtil;

/**
 *
 * @author dmicol
 */
public class NodeBtsCounterMapper extends Mapper<LongWritable, Text,
        ProtobufWritable<NodeBts>, NullWritable> {
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        context.write(NodeBtsUtil.wrap(NodeBtsUtil.parse(value.toString())),
                      NullWritable.get());
    }
}
