package es.tid.cosmos.mobility.labelling.join;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.MobDataUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <Long, TwoInt>
 * Output: <Long, Long>
 * 
 * @author dmicol
 */
public class ClusterGetMajPoiByNodeReducer extends Reducer<LongWritable,
        ProtobufWritable<MobData>, LongWritable, ProtobufWritable<MobData>> {
    @Override
    protected void reduce(LongWritable key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        TwoInt maxPoiblCount = null;
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final TwoInt poilblCount = value.get().getTwoInt();
            if (maxPoiblCount == null ||
                    poilblCount.getNum2() > maxPoiblCount.getNum2()) {
                maxPoiblCount = poilblCount;
            }
        }
        context.write(key, MobDataUtil.createAndWrap(maxPoiblCount.getNum1()));
    }
}
