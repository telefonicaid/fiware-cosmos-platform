package es.tid.ps.mobility.jobs;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.MxProtocol.BtsCounter;
import es.tid.ps.mobility.data.MxProtocol.NodeBts;

/**
 * @author sortega
 */
public class MobmxNodeBtsCounterReducer
        extends Reducer<ProtobufWritable<NodeBts>, NullWritable,
                        IntWritable, ProtobufWritable<BtsCounter>> {
    private IntWritable phone;
    private ProtobufWritable<BtsCounter> btsCounter;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.phone = new IntWritable();
        this.btsCounter = new ProtobufWritable<BtsCounter>();
        this.btsCounter.setConverter(BtsCounter.class);
    }

    @Override
    protected void reduce(ProtobufWritable<NodeBts> nodeBts,
                          Iterable<NullWritable> values,
                          Context context) throws IOException,
                                                  InterruptedException {
        int count = 0;
        for (NullWritable unused : values) {
            count++;
        }

        nodeBts.setConverter(NodeBts.class);
        this.phone.set(nodeBts.get().getPhone());
        this.btsCounter.set(BtsCounter.newBuilder()
                .setBts(nodeBts.get().getBts())
                .setWday(nodeBts.get().getWday())
                .setRange(nodeBts.get().getRange())
                .setCount(count)
                .build());
        context.write(this.phone, this.btsCounter);
    }
}
