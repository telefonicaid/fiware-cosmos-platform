package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;


/**
 * Input: <TwoInt, Null>
 * Output: <Long, TwoInt>
 * 
 * @author dmicol
 */
public class ClusterCountMajPoiByNodeReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>, LongWritable,
        ProtobufWritable<MobData>> {
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        int valueCount = 0;
        for (ProtobufWritable<MobData> value : values) {
            valueCount++;
        }
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        context.write(new LongWritable(twoInt.getNum1()),
                      MobDataUtil.createAndWrap(
                              TwoIntUtil.create(twoInt.getNum2(), valueCount)));
    }
}
