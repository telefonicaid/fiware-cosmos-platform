package es.tid.smartsteps.footfalls.microgrids.parsing;

import java.io.IOException;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.footfalls.microgrids.config.Config;
import es.tid.smartsteps.footfalls.microgrids.data.generated.EntryProtocol.Catchments;

/**
 *
 * @author dmicol
 */
public class CatchmentsParserMapper extends Mapper<
        LongWritable, Text,
        Text, TypedProtobufWritable<Catchments>> {

    private CatchmentsParser parser;
    private Set<String> dateToFilter;
    private Text outKey;
    private TypedProtobufWritable<Catchments> outValue;

    @Override
    protected void setup(Context context) {
        this.parser = new CatchmentsParser();
        this.dateToFilter = new HashSet<String>();
        final String[] dates = context.getConfiguration().getStrings(
                Config.DATES_TO_FILTER);
        if (dates != null) {
            this.dateToFilter.addAll(Arrays.asList(dates));
        }
        this.outKey = new Text();
        this.outValue = new TypedProtobufWritable<Catchments>();
    }

    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final Catchments catchments = this.parser.parse(value.toString());
        if (catchments == null) {
            context.getCounter(Counters.INVALID_CATCHMENTS).increment(1L);
            return;
        }
        if (!this.dateToFilter.isEmpty() &&
                !this.dateToFilter.contains(catchments.getDate())) {
            context.getCounter(Counters.FILTERED_CATCHMENTS).increment(1L);
            return;
        }
        this.outKey.set(catchments.getId());
        this.outValue.set(catchments);
        context.write(this.outKey, this.outValue);
    }
}
