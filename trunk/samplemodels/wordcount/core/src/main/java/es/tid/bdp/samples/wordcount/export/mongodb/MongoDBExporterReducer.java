package es.tid.bdp.samples.wordcount.export.mongodb;

import java.io.IOException;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

/**
 *
 * @author dmicol, dmicol, sortega
 */
public class MongoDBExporterReducer extends Reducer<LongWritable, Text,
                                                    Text, LongWritable> {
    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            String[] columns = value.toString().split("\t");
            context.write(new Text(columns[0]),
                          new LongWritable(Long.parseLong(columns[1])));
        }
    }
}
