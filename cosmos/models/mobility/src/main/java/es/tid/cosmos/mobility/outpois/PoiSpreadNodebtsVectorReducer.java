package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <NodeBts, ClusterVector>
 * Output: <TwoInt, ClusterVector>
 * 
 * @author dmicol
 */
public class PoiSpreadNodebtsVectorReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        for (ProtobufWritable<MobData> value : values) {
            context.write(TwoIntUtil.createAndWrap(nodeBts.getUserId(),
                                                   nodeBts.getPlaceId()), value);
        }
    }
}
