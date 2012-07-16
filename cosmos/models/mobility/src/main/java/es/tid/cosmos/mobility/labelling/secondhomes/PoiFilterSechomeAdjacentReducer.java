package es.tid.cosmos.mobility.labelling.secondhomes;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.data.generated.BaseTypes.Int64;
import es.tid.cosmos.base.data.generated.BaseTypes.Null;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Long>
 * Output: <TwoInt, Null>
 *
 * @author dmicol
 */
class PoiFilterSechomeAdjacentReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<InputIdRecord>,
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Null>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<InputIdRecord>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt pairbts = key.get();
        List<Long> nodeList = new LinkedList<Long>();
        for (TypedProtobufWritable<InputIdRecord> value : values) {
            final InputIdRecord record = value.get();
            if (record.getInputId() == 1) {
                return;
            }
            nodeList.add(Int64.parseFrom(record.getMessageBytes()).getValue());
        }
        for (long node : nodeList) {
            context.write(TwoIntUtil.createAndWrap(node, pairbts.getNum2()),
                          new TypedProtobufWritable<Null>(Null.getDefaultInstance()));
        }
    }
}
