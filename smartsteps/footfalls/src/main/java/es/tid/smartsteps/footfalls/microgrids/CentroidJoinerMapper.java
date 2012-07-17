package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.SOACentroid;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
class CentroidJoinerMapper extends Mapper<
        Text, TypedProtobufWritable<Message>,
        Text, TypedProtobufWritable<Message>> {

    private Text id;

    @Override
    protected void setup(Context context) {
        this.id = new Text();
    }

    @Override
    protected void map(Text key, TypedProtobufWritable<Message> value,
            Context context) throws IOException, InterruptedException {
        final Message message = value.get();
        if (message instanceof TrafficCounts) {
            // Here the cell ID should have been replaced by the SOA ID
            this.id.set(((TrafficCounts) message).getId());
        } else if (message instanceof SOACentroid) {
            this.id.set(((SOACentroid) message).getSoaId());
        } else {
            throw new IllegalStateException("Unexpected input: "
                                            + message.toString());
        }
        context.write(this.id, value);
    }
}
