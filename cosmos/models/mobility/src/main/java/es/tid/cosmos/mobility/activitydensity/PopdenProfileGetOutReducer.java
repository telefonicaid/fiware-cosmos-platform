package es.tid.cosmos.mobility.activitydensity;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.generated.MobProtocol.ClusterVector;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, ClusterVector>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class PopdenProfileGetOutReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<ClusterVector>,
        NullWritable, Text> {
    private String separator;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        final MobilityConfiguration conf = new MobilityConfiguration(context.
                getConfiguration());
        this.separator = conf.getDataSeparator();
    }
    
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<ClusterVector>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt btsProfile = key.get();
        for (TypedProtobufWritable<ClusterVector> value : values) {
            final ClusterVector commsVector = value.get();
            String str = btsProfile.getNum1() + this.separator
                         + btsProfile.getNum2();
            for (Double comm : commsVector.getComsList()) {
                str += this.separator + comm;
            }
            context.write(NullWritable.get(), new Text(str));
        }
    }
}
