package es.tid.smartsteps.dispersion.parsing;

import java.io.IOException;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.config.Config;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;

/**
 *
 * @author dmicol
 */
public class TrafficCountsParserMapper extends Mapper<
        LongWritable, Text,
        Text, TypedProtobufWritable<TrafficCounts>> {

    private TrafficCountsParser parser;
    private Set<String> dateToFilter;
    private Text outKey;
    private TypedProtobufWritable<TrafficCounts> outValue;

    @Override
    protected void setup(Context context) {
        this.parser = new TrafficCountsParser(
                context.getConfiguration().getStrings(Config.COUNT_FIELDS));
        this.dateToFilter = new HashSet<String>();
        final String[] dates = context.getConfiguration().getStrings(
                Config.DATES_TO_FILTER);
        if (dates != null) {
            this.dateToFilter.addAll(Arrays.asList(dates));
        }
        this.outKey = new Text();
        this.outValue = new TypedProtobufWritable<TrafficCounts>();
    }

    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final TrafficCounts counts = this.parser.parse(value.toString());
        if (counts == null) {
            context.getCounter(Counters.INVALID_TRAFFIC_COUNTS).increment(1L);
            return;
        }
        if (!this.dateToFilter.isEmpty() &&
                !this.dateToFilter.contains(counts.getDate())) {
            context.getCounter(Counters.FILTERED_TRAFFIC_COUNTS).increment(1L);
            return;
        }
        this.outKey.set(counts.getId());
        this.outValue.set(counts);
        context.write(this.outKey, this.outValue);
    }
}
