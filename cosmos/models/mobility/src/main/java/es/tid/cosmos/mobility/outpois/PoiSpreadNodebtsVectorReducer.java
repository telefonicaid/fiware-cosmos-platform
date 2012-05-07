package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.MobProtocol.NodeBts;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class PoiSpreadNodebtsVectorReducer extends Reducer<
        ProtobufWritable<NodeBts>, ProtobufWritable<ClusterVector>,
        ProtobufWritable<TwoInt>, ProtobufWritable<ClusterVector>> {
    @Override
    protected void reduce(ProtobufWritable<NodeBts> key,
            Iterable<ProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(NodeBts.class);
        final NodeBts nodeBts = key.get();
        for (ProtobufWritable<ClusterVector> value : values) {
            context.write(TwoIntUtil.createAndWrap(nodeBts.getUserId(),
                                                   nodeBts.getPlaceId()), value);
        }
    }
}
