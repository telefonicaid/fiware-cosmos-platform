package es.tid.bdp.kpicalculation.export.mongodb;

import java.io.IOException;
import java.util.Arrays;
import java.util.ArrayList;
import java.util.List;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

/**
 *
 * @author dmicol, sortega
 */
public class MongoDBExporterReducer extends Reducer<LongWritable, Text,
        NullWritable, BSONWritable> {
    @Override
    public void reduce(LongWritable key, Iterable<Text> values,
                       Context context) throws IOException,
                                               InterruptedException {
        for (Text value : values) {
            List<String> columns = new ArrayList<String>();
            columns.addAll(Arrays.asList(value.toString().split("\t")));
            List<String> attributes = columns.subList(0, columns.size() - 1);
            int count = Integer.parseInt(columns.get(columns.size() - 1));
            context.write(NullWritable.get(),
                          toBSON((String[]) attributes.toArray(),
                                 new IntWritable(count)));
        }
    }

    private BSONWritable toBSON(String[] attributes, IntWritable count) {
        BSONObject obj = new BasicBSONObject();
        obj.put("attributes", attributes);
        obj.put("count", count);
        return new BSONWritable(obj);
    }
}
