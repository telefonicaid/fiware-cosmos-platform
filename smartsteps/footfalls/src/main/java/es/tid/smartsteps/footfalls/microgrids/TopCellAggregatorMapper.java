package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.cosmos.base.mapreduce.BinaryKey;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;

/**
 * Keys a number of Catchments containing a single TopCell with its top level
 * id and date.
 *
 * In: <id, Catchments>
 * Out: <[id, date], Catchments>
 *
 * @author sortega
 */
public class TopCellAggregatorMapper extends Mapper<
        Text, TypedProtobufWritable<Catchments>,
        BinaryKey, TypedProtobufWritable<Catchments>> {

    private BinaryKey outKey;

    @Override
    protected void setup(Context context) {
        this.outKey = new BinaryKey();
    }

    @Override
    protected void map(Text key,
                       TypedProtobufWritable<Catchments> value, Context context)
            throws IOException, InterruptedException {
        final Catchments catchments = value.get();
        this.outKey.setPrimaryKey(catchments.getId());
        this.outKey.setSecondaryKey(catchments.getDate());
        context.write(this.outKey, value);
    }
}
