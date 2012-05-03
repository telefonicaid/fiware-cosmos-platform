package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class AdjSwapPoiIdSt2Reducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> pairPoisList = new LinkedList<TwoInt>();
        TwoInt pairIndex = null;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getInputId()) {
                case 0:
                    pairPoisList.add(mobData.getTwoInt());
                    break;
                case 1:
                    // Only keep the last element of this list
                    pairIndex = mobData.getTwoInt();
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData ID: "
                            + mobData.getInputId());
            }
        }
        
        for (TwoInt pairPois : pairPoisList) {
            TwoInt.Builder outputPairPois = TwoInt.newBuilder(pairPois);
            if (pairIndex != null) {
                outputPairPois.setNum2(pairIndex.getNum2());
            }
            if (outputPairPois.getNum1() == outputPairPois.getNum2()) {
                continue;
            } else if (outputPairPois.getNum1() > outputPairPois.getNum2()) {
                long tmp = outputPairPois.getNum1();
                outputPairPois.setNum1(outputPairPois.getNum2());
                outputPairPois.setNum2(tmp);
            }
            context.write(new LongWritable(outputPairPois.getNum1()),
                          MobDataUtil.createAndWrap(outputPairPois.build()));
        }
    }
}
