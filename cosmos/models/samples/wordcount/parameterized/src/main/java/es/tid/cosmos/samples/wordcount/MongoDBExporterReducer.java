package es.tid.cosmos.samples.wordcount;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 * Export wordcount results to MongoDB
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterReducer extends Reducer<LongWritable, Text,
                                                    Text, LongWritable> {
    private static final String DELIMITER = "\t";
    
    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            String[] columns = value.toString().split(DELIMITER);
            context.write(new Text(columns[0]),
                          new LongWritable(Long.parseLong(columns[1])));
        }
    }
}
