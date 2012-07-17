package es.tid.smartsteps.footfalls.trafficcounts;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
public class TrafficCountsScalerMapper extends Mapper<
        Text, TypedProtobufWritable<Message>,
        Text, TypedProtobufWritable<Message>> {

    private Text cellId;

    @Override
    protected void setup(Context context) {
        this.cellId = new Text();
    }

    @Override
    protected void map(Text key, TypedProtobufWritable<Message> value,
            Context context) throws IOException, InterruptedException {
        final Message message = value.get();
        if (message instanceof TrafficCounts) {
            this.cellId.set(((TrafficCounts) message).getId());
        } else if (message instanceof Lookup) {
            this.cellId.set(((Lookup) message).getKey());
        } else {
            throw new IllegalStateException("Unexpected input: "
                                            + message.toString());
        }
        context.write(this.cellId, value);
    }
}
