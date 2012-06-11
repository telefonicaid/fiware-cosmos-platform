package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobilityWritable;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Null;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;


/**
 * Input: <TwoInt, Null>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class ClusterCountMajPoiByNodeReducer extends Reducer<
        ProtobufWritable<TwoInt>, MobilityWritable<Null>, LongWritable,
        MobilityWritable<TwoInt>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<MobilityWritable<Null>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (MobilityWritable<Null> value : values) {
            valueCount++;
        }
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        context.write(new LongWritable(twoInt.getNum1()),
                      new MobilityWritable<TwoInt>(
                              TwoIntUtil.create(twoInt.getNum2(), valueCount)));
    }
}
