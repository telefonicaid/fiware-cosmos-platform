package es.tid.smartsteps.footfalls.microgrids;

import java.io.IOException;

import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.config.Config;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;
import es.tid.smartsteps.footfalls.microgrids.parsing.CatchmentsParser;

/**
 *
 * @author sortega
 */
public class CatchmentsJsonExporterReducer extends Reducer<
        Text, TypedProtobufWritable<Catchments>,
        NullWritable, Text> {

    private boolean shouldRoundResults;
    private Text outValue;
    final private CatchmentsParser parser;

    /**
     * Construct with default dependencies
     */
    public CatchmentsJsonExporterReducer() {
        this(new CatchmentsParser());
    }

    CatchmentsJsonExporterReducer(CatchmentsParser parser) {
        this.parser = parser;
    }

    @Override
    protected void setup(Context context) {
        this.shouldRoundResults = context.getConfiguration().getBoolean(
                Config.ROUND_RESULTS, false);
        this.outValue = new Text();
    }

    @Override
    protected void reduce(Text key,
            Iterable<TypedProtobufWritable<Catchments>> values,
            Context context) throws IOException, InterruptedException {
        for (TypedProtobufWritable<Catchments> value : values) {
            this.outValue.set(this.parser
                    .toJSON(value.get(), this.shouldRoundResults)
                    .toString());
            context.write(NullWritable.get(), this.outValue);
        }
    }
}
