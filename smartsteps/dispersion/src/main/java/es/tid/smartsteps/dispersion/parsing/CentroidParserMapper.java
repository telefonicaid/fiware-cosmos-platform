package es.tid.smartsteps.dispersion.parsing;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

import es.tid.cosmos.base.data.TypedProtobufWritable;
import es.tid.smartsteps.dispersion.config.Config;
import es.tid.smartsteps.dispersion.data.generated.EntryProtocol.SOACentroid;

/**
 *
 * @author dmicol
 */
public class CentroidParserMapper extends Mapper<
        LongWritable, Text,
        Text, TypedProtobufWritable<SOACentroid>> {

    private CentroidParser parser;
    private Text outKey;
    private TypedProtobufWritable<SOACentroid> outValue;
    
    @Override
    protected void setup(Context context) {
        this.parser = new CentroidParser(
                context.getConfiguration().get(Config.DELIMITER));
        this.outKey = new Text();
        this.outValue = new TypedProtobufWritable<SOACentroid>();
    }
    
    @Override
    protected void map(LongWritable key, Text value, Context context)
            throws IOException, InterruptedException {
        final SOACentroid soaCentroid = this.parser.parse(value.toString());
        if (soaCentroid == null) {
            context.getCounter(Counters.INVALID_SOA_CENTROIDS).increment(1L);
            return;
        }
        this.outKey.set(soaCentroid.getSoaId());
        this.outValue.set(soaCentroid);
        context.write(this.outKey, this.outValue);
    }
}
