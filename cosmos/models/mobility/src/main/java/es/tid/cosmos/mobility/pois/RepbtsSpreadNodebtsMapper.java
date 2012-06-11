package es.tid.cosmos.mobility.pois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.NodeBtsDayUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.BtsCounter;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBtsDay;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeMxCounter;

/**
 * Input: <LongWritable, NodeMxCounter>
 * Output: <NodeBtsDay, Int>
 * 
 * @author dmicol
 */
public class RepbtsSpreadNodebtsMapper extends Mapper<LongWritable,
        ProtobufWritable<MobData>, ProtobufWritable<NodeBtsDay>,
        ProtobufWritable<MobData>> {
    @Override
    public void map(LongWritable key, ProtobufWritable<MobData> value,
            Context context) throws IOException, InterruptedException {
        value.setConverter(MobData.class);
        final NodeMxCounter counter = value.get().getNodeMxCounter();
        for (BtsCounter bts : counter.getBtsList()) {
            ProtobufWritable<NodeBtsDay> nodeBtsDay = NodeBtsDayUtil.
                    createAndWrap(key.get(), bts.getBts(), 0, 0);
            context.write(nodeBtsDay,
                          MobDataUtil.createAndWrap(bts.getCount()));
        }
    }
}
