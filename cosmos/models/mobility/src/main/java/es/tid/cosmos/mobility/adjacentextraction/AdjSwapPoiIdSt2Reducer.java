package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.google.protobuf.ByteString;
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
class AdjSwapPoiIdSt2Reducer extends Reducer<LongWritable,
        TypedProtobufWritable<InputIdRecord>, LongWritable, TypedProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<TypedProtobufWritable<InputIdRecord>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> pairPoisList = new LinkedList<TwoInt>();
        TwoInt lastPairIndex = null;
        for (TypedProtobufWritable<InputIdRecord> value : values) {
            final InputIdRecord record = value.get();
            final TwoInt twoInt = TwoInt.parseFrom(record.getMessageBytes());
            switch (record.getInputId()) {
                case 0:
                    pairPoisList.add(twoInt);
                    break;
                case 1:
                    lastPairIndex = twoInt;
                    break;
                default:
                    throw new IllegalStateException("Unexpected Input ID: "
                            + record.getInputId());
            }
        }
        
        for (TwoInt pairPois : pairPoisList) {
            TwoInt.Builder outputPairPois = TwoInt.newBuilder(pairPois);
            if (lastPairIndex != null) {
                outputPairPois.setNum2(lastPairIndex.getNum2());
            }
            if (outputPairPois.getNum1() == outputPairPois.getNum2()) {
                continue;
            } else if (outputPairPois.getNum1() > outputPairPois.getNum2()) {
                long tmp = outputPairPois.getNum1();
                outputPairPois.setNum1(outputPairPois.getNum2());
                outputPairPois.setNum2(tmp);
            }
            context.write(new LongWritable(outputPairPois.getNum1()),
                          new TypedProtobufWritable<TwoInt>(outputPairPois.build()));
        }
    }
}
