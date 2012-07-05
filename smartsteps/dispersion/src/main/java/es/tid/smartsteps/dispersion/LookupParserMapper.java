package es.tid.smartsteps.dispersion;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.data.generated.LookupProtocol.Lookup;
import es.tid.smartsteps.dispersion.parsing.LookupParser;

/**
 *
 * @author dmicol
 */
class LookupParserMapper extends Mapper<LongWritable, Text,
                                        Text, TypedProtobufWritable<Lookup>> {

    private LookupParser parser;
    private Text outKey;
    private TypedProtobufWritable<Lookup> outValue;
    
    @Override
    protected void setup(Context context) throws IOException,
                                                 InterruptedException {
        this.parser = new LookupParser(
                context.getConfiguration().get(Config.DELIMITER));
        this.outKey = new Text();
        this.outValue = new TypedProtobufWritable<Lookup>();
    }
    
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final Lookup lookup = this.parser.parse(value.toString());
        if (lookup == null) {
            context.getCounter(Counters.INVALID_LOOKUPS).increment(1L);
            return;
        }
        this.outKey.set(lookup.getKey());
        this.outValue.set(lookup);
        context.write(this.outKey, this.outValue);
    }
}
