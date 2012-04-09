package es.tid.cosmos.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;

/**
 *
 * @author dmicol
 */
public class NodeBtsCounterMapper extends Mapper<LongWritable,
        ProtobufWritable<NodeBts>, ProtobufWritable<NodeBts>, NullWritable> {
    @Override
    protected void map(LongWritable key, ProtobufWritable<NodeBts> value,
            Context context) throws IOException, InterruptedException {
        context.write(value, NullWritable.get());
    }
}
