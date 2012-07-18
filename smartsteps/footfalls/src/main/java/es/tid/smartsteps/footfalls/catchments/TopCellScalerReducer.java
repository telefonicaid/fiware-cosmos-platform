package es.tid.smartsteps.footfalls.catchments;

import java.io.IOException;
import java.util.List;
import java.util.Map;

import com.google.protobuf.Message;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchment;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.data.generated.EntryProtocol.TopCell;
import es.tid.smartsteps.footfalls.data.generated.LookupProtocol.Lookup;

/**
 * Scale TopCell counts and translate id by means of lookup records.
 *
 * n x m output records are expected where n is the number of lookups and
 * m is the number of catchments for a given cell id.
 *
 * In: <id, Lookup> | <id, Catchemt>
 * Out: <id, Catchment>
 *
 * @author sortega
 */
public class TopCellScalerReducer extends Reducer<
        Text, TypedProtobufWritable<Message>,
        Text, TypedProtobufWritable<Catchments>> {

    private TypedProtobufWritable<Catchments> outValue;

    @Override
    protected void setup(Context context) {
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
                this.outValue.set(scaleCatchments(catchments, lookup));
                context.write(key, this.outValue);
            }
        }
    }

    private static Catchments scaleCatchments(Catchments input, Lookup lookup) {
        final TopCell inputTopCell = input.getCatchments(0).getTopCells(0);
        final double scaledCount = lookup.getProportion() *
                                   inputTopCell.getCount();
        TopCell.Builder scaledTopCell = TopCell.newBuilder()
                .setId(lookup.getValue())
                .setCount(scaledCount)
                .setLatitude(inputTopCell.getLatitude())
                .setLongitude(inputTopCell.getLongitude());
        Catchment.Builder scaledCatchment = Catchment.newBuilder()
                .setHour(input.getCatchments(0).getHour())
                .addTopCells(scaledTopCell);
        return Catchments.newBuilder(input)
                .setCatchments(0, scaledCatchment)
                .build();
    }
}
