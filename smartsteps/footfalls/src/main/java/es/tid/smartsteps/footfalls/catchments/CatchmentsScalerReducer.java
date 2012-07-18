package es.tid.smartsteps.footfalls.catchments;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 *
 * @author dmicol
 */
public class CatchmentsScalerReducer extends Reducer<
        Text, TypedProtobufWritable<Message>,
        Text, TypedProtobufWritable<Catchments>> {

    private Text outKey;
    private TypedProtobufWritable<Catchments> outValue;

    @Override
    protected void setup(Context context) {
        this.outKey = new Text();
        this.outValue = new TypedProtobufWritable<Catchments>();
    }

    @Override
    protected void reduce(Text key,
            Iterable<TypedProtobufWritable<Message>> values, Context context)
            throws IOException, InterruptedException {
        Map<Class, List> dividedLists = TypedProtobufWritable.groupByClass(
                values, Catchments.class, Lookup.class);
        List<Catchments> catchmentsList = dividedLists.get(Catchments.class);
        List<Lookup> lookups = dividedLists.get(Lookup.class);
        if (lookups.isEmpty()) {
            context.getCounter(Counters.ENTRIES_NOT_IN_LOOKUP)
                   .increment(catchmentsList.size());
            return;
        }
        context.getCounter(Counters.ENTRIES_IN_LOOKUP)
               .increment(catchmentsList.size());

        for (Catchments catchments : catchmentsList) {
            for (Lookup lookup : lookups) {
                final Catchments scaledCatchments =
                        CatchmentUtil.scaleCatchments(catchments, lookup);
                this.outKey.set(scaledCatchments.getId());
                this.outValue.set(scaledCatchments);
                context.write(this.outKey, this.outValue);
            }
        }
    }
}
