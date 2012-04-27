package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;

/**
 *
 * @author dmicol
 */
public class AdjSwapPoiIdSt2Reducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> pairPoisList = new LinkedList<TwoInt>();
        LinkedList<TwoInt> pairIndexList = new LinkedList<TwoInt>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getInputId()) {
                case 0:
                    pairPoisList.add(mobData.getTwoInt());
                    break;
                case 1:
                    pairIndexList.add(mobData.getTwoInt());
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData ID: "
                            + mobData.getInputId());
            }
        }
        
        for (TwoInt pairPois : pairPoisList) {
            TwoInt.Builder outputPairPois = TwoInt.newBuilder(pairPois);
            if (!pairIndexList.isEmpty()) {
                outputPairPois.setNum2(pairIndexList.getLast().getNum2());
            }
            if (outputPairPois.getNum1() == outputPairPois.getNum2()) {
                continue;
            } else if (outputPairPois.getNum1() > outputPairPois.getNum2()) {
                long tmp = outputPairPois.getNum1();
                outputPairPois.setNum1(outputPairPois.getNum2());
                outputPairPois.setNum2(tmp);
            }
            context.write(new LongWritable(outputPairPois.getNum1()),
                          TwoIntUtil.wrap(outputPairPois.build()));
        }
    }
}
