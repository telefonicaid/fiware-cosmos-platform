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
 * Input: <Long, TwoInt>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class AdjUpdatePoisTableReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> poiPoimodList = new LinkedList<TwoInt>();
        TwoInt lastPairPois = null;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getInputId()) {
                case 0:
                    poiPoimodList.add(mobData.getTwoInt());
                    break;
                case 1:
                    lastPairPois = mobData.getTwoInt();
                    break;
                default:
                    throw new IllegalStateException("Unexpected MobData ID: "
                            + mobData.getInputId());
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
                          MobDataUtil.createAndWrap(outputPoiPoimod.build()));
        }
    }
}
