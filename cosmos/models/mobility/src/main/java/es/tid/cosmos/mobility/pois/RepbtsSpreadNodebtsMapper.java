package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.MobProtocol.NodeMxCounter;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;

/**
 *
 * @author dmicol
 */
public class RepbtsSpreadNodebtsMapper extends Mapper<LongWritable,
        ProtobufWritable<NodeMxCounter>, ProtobufWritable<NodeBtsDay>,
        IntWritable> {
    private IntWritable numberOfCalls;
    
    @Override
    public void setup(Context context) {
        this.numberOfCalls = new IntWritable();
    }
    
    @Override
    public void map(LongWritable key, ProtobufWritable<NodeMxCounter> value,
            Context context) throws IOException, InterruptedException {
        final NodeMxCounter counter = value.get();
        for (BtsCounter bts : counter.getBtsList()) {
            ProtobufWritable<NodeBtsDay> nodeBtsDay = NodeBtsDayUtil.
                    createAndWrap(key.get(), bts.getPlaceId(), 0, 0);
            this.numberOfCalls.set(bts.getCount());
            context.write(nodeBtsDay, this.numberOfCalls);
        }
    }
}
