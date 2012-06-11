package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Int;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <LongWritable, NodeMxCounter>
 * Output: <NodeBtsDay, Int>
 * 
 * @author dmicol
 */
public class RepbtsSpreadNodebtsMapper extends Mapper<LongWritable,
        MobilityWritable<NodeMxCounter>, ProtobufWritable<NodeBtsDay>,
        MobilityWritable<Int>> {
    @Override
    public void map(LongWritable key, MobilityWritable<NodeMxCounter> value,
            Context context) throws IOException, InterruptedException {
        final NodeMxCounter counter = value.get();
        for (BtsCounter bts : counter.getBtsList()) {
            ProtobufWritable<NodeBtsDay> nodeBtsDay = NodeBtsDayUtil.
                    createAndWrap(key.get(), bts.getBts(), 0, 0);
            context.write(nodeBtsDay, MobilityWritable.create(bts.getCount()));
        }
    }
}
