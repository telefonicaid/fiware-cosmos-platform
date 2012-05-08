package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 * Input: <TwoInt, Long|Null>
 * Output: <TwoInt, Null>
 * 
 * @author dmicol
 */
public class PoiFilterSechomeAdjacentReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<Long> nodeList = new LinkedList<Long>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getType()) {
                case LONG:
                    nodeList.add(mobData.getLong());
                    break;
                case NULL:
                    // If we find one null, we won't output anything
                    return;
                default:
                    throw new IllegalStateException("Unexpected MobData type: "
                            + mobData.getType().name());
            }
        }

        key.setConverter(TwoInt.class);
        final TwoInt pairbts = key.get();
        for (long node : nodeList) {
            context.write(TwoIntUtil.createAndWrap(node, pairbts.getNum2()),
                          MobDataUtil.createAndWrap(NullWritable.get()));
        }
    }
}
