package es.tid.ps.mobility.mapreduce;

import java.io.IOException;

import com.twitter.elephantbird.mapreduce.io.ProtobufWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.ps.mobility.data.MobProtocol.BtsCounter;
import es.tid.ps.mobility.data.MobProtocol.NodeBts;

/**
 * @author sortega
 */
public class NodeBtsCounterReducer
        extends Reducer<ProtobufWritable<NodeBts>, NullWritable,
                        LongWritable, ProtobufWritable<BtsCounter>> {
    private LongWritable userId;
    private ProtobufWritable<BtsCounter> btsCounter;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.userId = new LongWritable();
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
        this.userId.set(nodeBts.get().getUserId());
        this.btsCounter.set(BtsCounter.newBuilder()
                .setPlaceId(nodeBts.get().getPlaceId())
                .setWeekday(nodeBts.get().getWeekday())
                .setRange(nodeBts.get().getRange())
                .setCount(count)
                .build());
        context.write(this.userId, this.btsCounter);
    }
}
