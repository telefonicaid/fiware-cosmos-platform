package es.tid.cosmos.mobility.util;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.Config;
import es.tid.cosmos.mobility.data.ClusterUtil;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Cluster>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
public class ExportClusterToTextByTwoIntReducer extends Reducer<
        ProtobufWritable<TwoInt>, TypedProtobufWritable<Cluster>, NullWritable,
        Text> {
    private String separator;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.separator = context.getConfiguration().get(Config.DATA_SEPARATOR);
    }
    
    @Override
    protected void reduce(ProtobufWritable<TwoInt> key,
            Iterable<TypedProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (TypedProtobufWritable<Cluster> value : values) {
            final Cluster cluster = value.get();
            context.write(NullWritable.get(),
                          new Text(TwoIntUtil.toString(twoInt, this.separator)
                                   + this.separator
                                   + ClusterUtil.toString(cluster,
                                                          this.separator)));
        }
    }
}
