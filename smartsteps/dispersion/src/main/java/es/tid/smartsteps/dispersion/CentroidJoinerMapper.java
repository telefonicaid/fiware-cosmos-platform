package es.tid.smartsteps.dispersion;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.SOACentroid;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
class CentroidJoinerMapper extends Mapper<
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
            // Here the cell ID should have been replaced by the SOA ID
            this.cellId.set(((TrafficCounts) message).getCellId());
        } else if (message instanceof SOACentroid) {
            this.cellId.set(((SOACentroid) message).getSoaId());
        } else {
            throw new IllegalStateException("Unexpected input: "
                                            + message.toString());
        }
        context.write(this.cellId, value);
    }
}
