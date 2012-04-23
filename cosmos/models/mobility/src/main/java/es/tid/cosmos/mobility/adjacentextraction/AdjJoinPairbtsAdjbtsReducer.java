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
public class AdjJoinPairbtsAdjbtsReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        LongWritable, ProtobufWritable<TwoInt>> {
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
                    throw new IllegalStateException();
            }
        }
        
        if (hasAdjacentBts) {
            for (TwoInt pairPois : pairPoisList) {
                context.write(new LongWritable(pairPois.getNum1()),
                            TwoIntUtil.wrap(pairPois));
            }
        }
    }
}
