package es.tid.bdp.samples.wordcount.export.mongodb;

import java.io.IOException;

import com.mongodb.hadoop.io.BSONWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import org.bson.BSONObject;
import org.bson.BasicBSONObject;

/**
 *
 * @author dmicol, dmicol, sortega
 */
public class MongoDBExporterReducer extends Reducer<LongWritable, Text,
                                                    NullWritable,
                                                    BSONWritable> {
    @Override
    public void reduce(LongWritable key, Iterable<Text> values, Context context)
            throws IOException, InterruptedException {
        for (Text value : values) {
            context.write(NullWritable.get(), this.toBSON(value));
        }
    }

    private BSONWritable toBSON(Text value) {
        String[] columns = value.toString().split("\t");
        BSONObject obj = new BasicBSONObject();
        obj.put("word", columns[0]);
        obj.put("count", Long.parseLong(columns[1]));
        return new BSONWritable(obj);
    }
}
