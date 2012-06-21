package es.tid.cosmos.mobility.outpois;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.mobility.MobilityConfiguration;
import es.tid.cosmos.mobility.data.TwoIntUtil;
import es.tid.cosmos.mobility.data.generated.MobProtocol.Cluster;
import es.tid.cosmos.mobility.data.generated.MobProtocol.TwoInt;

/**
 * Input: <TwoInt, Cluster>
 * Output: <Null, Text>
 * 
 * @author dmicol
 */
class VectorNodbtsOutReducer extends Reducer<ProtobufWritable<TwoInt>,
        TypedProtobufWritable<Cluster>, NullWritable, Text> {
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
            Iterable<TypedProtobufWritable<Cluster>> values, Context context)
            throws IOException, InterruptedException {
        key.setConverter(TwoInt.class);
        final TwoInt twoInt = key.get();
        for (TypedProtobufWritable<Cluster> value : values) {
            final Cluster cluster = value.get();
            String output =
                    TwoIntUtil.toString(twoInt, this.separator) + this.separator
                    + cluster.getLabel() + this.separator
                    + cluster.getLabelgroup() + this.separator
                    + cluster.getConfident();
            for (double comm : cluster.getCoords().getComsList()) {
                output += this.separator + comm;
            }
            context.write(NullWritable.get(), new Text(output));
        }
    }
}
