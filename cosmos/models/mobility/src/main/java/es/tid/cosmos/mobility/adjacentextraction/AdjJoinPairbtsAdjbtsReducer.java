package es.tid.cosmos.mobility.adjacentextraction;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, TwoInt>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class AdjJoinPairbtsAdjbtsReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> pairPoisList = new LinkedList<TwoInt>();
        boolean hasAdjacentBts = false;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getInputId()) {
                case 0:
                    pairPoisList.add(mobData.getTwoInt());
                    break;
                case 1:
                    hasAdjacentBts = true;
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData ID: "
                            + mobData.getInputId());
            }
        }
        
        if (hasAdjacentBts) {
            for (TwoInt pairPois : pairPoisList) {
                context.write(new LongWritable(pairPois.getNum1()),
                              MobDataUtil.createAndWrap(pairPois));
            }
        }
    }
}
