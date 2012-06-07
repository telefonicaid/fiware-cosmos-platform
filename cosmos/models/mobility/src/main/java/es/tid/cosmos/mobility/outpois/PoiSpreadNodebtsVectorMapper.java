package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <TwoInt, ClusterVector>
 * 
 * @author dmicol
 */
public class PoiSpreadNodebtsVectorMapper extends Mapper<
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void map(ProtobufWritable<NodeBts> key,
            ProtobufWritable<MobData> value, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        context.write(TwoIntUtil.createAndWrap(nodeBts.getUserId(),
                                               nodeBts.getBts()), value);
    }
}
