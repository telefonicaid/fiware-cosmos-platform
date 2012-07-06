package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.TrafficCounts;
import es.tid.smartsteps.dispersion.parsing.TrafficCountsParser;

/**
 *
 * @author dmicol
 */
class TrafficCountsParserMapper extends Mapper<
        LongWritable, Text,
        Text, TypedProtobufWritable<TrafficCounts>> {

    private TrafficCountsParser parser;
    private Text outKey;
    private TypedProtobufWritable<TrafficCounts> outValue;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.parser = new TrafficCountsParser(
                context.getConfiguration().getStrings(Config.COUNT_FIELDS));
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
        this.outKey.set(counts.getCellId());
        this.outValue.set(counts);
        context.write(this.outKey, this.outValue);
    }
}
