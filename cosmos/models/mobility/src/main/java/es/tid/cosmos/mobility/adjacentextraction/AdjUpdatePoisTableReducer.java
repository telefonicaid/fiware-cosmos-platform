package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.data.generated.MobProtocol.InputIdRecord;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
class AdjUpdatePoisTableReducer extends Reducer<LongWritable,
        TypedProtobufWritable<InputIdRecord>, LongWritable, TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<InputIdRecord>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> poiPoimodList = new LinkedList<TwoInt>();
        TwoInt lastPairPois = null;
        for (TypedProtobufWritable<InputIdRecord> value : values) {
            final InputIdRecord record = value.get();
            final TwoInt twoInt = TwoInt.parseFrom(record.getMessageBytes());
            switch (record.getInputId()) {
                case 0:
                    poiPoimodList.add(twoInt);
                    break;
                case 1:
                    lastPairPois = twoInt;
                    break;
                default:
                    throw new IllegalStateException("Unexpected Input ID: "
                            + record.getInputId());
            }
        }
        for (TwoInt poiPoimod : poiPoimodList) {
            long outputKey = key.get();
            TwoInt.Builder outputPoiPoimod = TwoInt.newBuilder(poiPoimod);
            if (lastPairPois != null) {
                outputKey = lastPairPois.getNum2();
                outputPoiPoimod.setNum2(outputKey);
            }
            context.write(new LongWritable(outputKey),
                          new TypedProtobufWritable<TwoInt>(outputPoiPoimod.build()));
        }
    }
}
