package es.tid.smartsteps.footfalls.catchments;

import java.io.IOException;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TopCell;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 * Maps lookups to their source id and partial Catchments to their TopCell id.
 * There will be an output Catchments per input TopCell.
 *
 * In: <key, Lookup> | <key, Catchments>
 * Out: <id, Lookup> | <id, Catchemt>
 *
 * @author sortega
 */
public class TopCellScalerMapper extends Mapper<
        Text, TypedProtobufWritable<Message>,
        Text, TypedProtobufWritable<Message>> {

    private Text id;

    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.id = new Text();
    }

    @Override
    protected void map(Text key,
                       TypedProtobufWritable<Message> value,
                       Context context)
            throws IOException, InterruptedException {

        final Message message = value.get();
        if (message instanceof Lookup) {
            this.id.set(((Lookup) message).getKey());
            context.write(this.id, value);
        } else if (message instanceof Catchments) {
            this.emitTopCells((Catchments) message, context);
        } else {
            throw new IllegalStateException("Unexpected input: " +
                                            message.toString());
        }
    }

    private void emitTopCells(Catchments catchments, Context context)
            throws IOException, InterruptedException {
        Catchments.Builder catchmentsPrototype = Catchments
                .newBuilder(catchments)
                .clearCatchments();
        for (Catchment catchment : catchments.getCatchmentsList()) {
            int hour = catchment.getHour();
            for (TopCell topCell : catchment.getTopCellsList()) {
                Catchments outCatchments = catchmentsPrototype.clone()
                        .addCatchments(Catchment.newBuilder()
                                .setHour(hour)
                                .addTopCells(topCell))
                        .build();
                this.id.set(topCell.getId());
                context.write(this.id, new TypedProtobufWritable<Message>(
                        outCatchments));
            }
        }
    }
}
