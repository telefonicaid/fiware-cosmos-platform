package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.MobData;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, ClusterVector>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class PopdenProfileGetOutReducer extends Reducer<
        ProtobufWritable<TwoInt>, ProtobufWritable<MobData>,
        NullWritable, Text> {
    private static final String DELIMITER = "|";
    
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<ProtobufWritable<MobData>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt btsProfile = key.get();
        for (ProtobufWritable<MobData> value : values) {
            value.setConverter(MobData.class);
            final MobData mobData = value.get();
            final ClusterVector commsVector = mobData.getClusterVector();
            String str = btsProfile.getNum1() + DELIMITER
                         + btsProfile.getNum2();
            for (Double comm : commsVector.getComsList()) {
                str += DELIMITER + comm;
            }
            context.write(NullWritable.get(), new Text(str));
        }
    }
}
