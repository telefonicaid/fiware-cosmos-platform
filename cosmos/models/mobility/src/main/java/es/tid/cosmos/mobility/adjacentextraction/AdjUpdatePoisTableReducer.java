package es.tid.cosmos.mobility.adjacentextraction;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.MobProtocol.TwoInt;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

/**
 *
 * @author dmicol
 */
public class AdjUpdatePoisTableReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<TwoInt>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        List<TwoInt> poiPoimodList = new LinkedList<TwoInt>();
        List<TwoInt> pairPoisList = new LinkedList<TwoInt>();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            switch (mobData.getInputId()) {
                case 0:
                    poiPoimodList.add(mobData.getTwoInt());
                    break;
                case 1:
                    pairPoisList.add(mobData.getTwoInt());
                    break;
                default:
                    throw new IllegalStateException();
            }
        }
        
        for (TwoInt poiPoimod : poiPoimodList) {
            TwoInt.Builder outputPoiPoimod = TwoInt.newBuilder(poiPoimod);
            for (TwoInt pairPois : pairPoisList) {
                outputPoiPoimod.setNum2(pairPois.getNum2());
            }
            context.write(new LongWritable(outputPoiPoimod.getNum2()),
                          TwoIntUtil.wrap(outputPoiPoimod.build()));
        }
    }
}
