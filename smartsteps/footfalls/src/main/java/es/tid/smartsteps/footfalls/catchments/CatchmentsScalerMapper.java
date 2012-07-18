package es.tid.smartsteps.footfalls.catchments;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
public class CatchmentsScalerMapper extends Mapper<
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
        if (message instanceof Catchments) {
            this.cellId.set(((Catchments) message).getId());
        } else if (message instanceof Lookup) {
            this.cellId.set(((Lookup) message).getKey());
        } else {
            throw new IllegalStateException("Unexpected input: "
                                            + message.toString());
        }
        context.write(this.cellId, value);
    }
}
